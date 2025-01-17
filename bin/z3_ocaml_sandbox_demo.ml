(* open Z3_mini

let () =
  let _ = z3_mini_ctx in
  print_endline "done" *)
module StringMap = Map.Make (String)
open Domainslib

(* Function to read file contents *)
let read_file path =
  let ic = open_in path in
  let len = in_channel_length ic in
  let contents = really_input_string ic len in
  close_in ic;
  contents

(* Recursively walk through a directory and build a map of file paths to contents *)
let rec collect_files_as_map dir acc =
  let entries = Sys.readdir dir in
  Array.fold_left
    (fun acc entry ->
      let path = Filename.concat dir entry in
      if Sys.is_directory path then collect_files_as_map path acc
      else if Filename.check_suffix path ".smt2" then
        let content = read_file path in
        StringMap.add path content acc
      else acc)
    acc entries

(* Count the number of newlines in a string *)
let count_newlines content =
  String.fold_left (fun acc c -> if c = '\n' then acc + 1 else acc) 0 content

(* Parallel newline counting with configurable chunk size *)
let parallel_count_newlines file_map chunk_size num_domains =
  let total_files = StringMap.cardinal file_map in
  let pool = Domainslib.Task.setup_pool ~num_domains () in

  (* Convert file_map to an array for indexed access *)
  let files = Array.of_list (StringMap.bindings file_map) in

  (* Use parallel_for_reduce to process files in parallel *)
  let results =
    Domainslib.Task.run pool (fun () ->
        (* Domainslib.Task.parallel_for_reduce ~chunk_size ~start:0
          ~finish:(total_files - 1) ~pool
          ~body:(fun _i -> 1) (* Return a single tuple *)
          ~reduce:( + ) (* Prepend to the list *)
          ~init:0) *)
        Domainslib.Task.parallel_for_reduce pool ( + ) 0 ~chunk_size ~start:0
          ~finish:(total_files - 1) ~body:(fun _i -> 1))
    (* Start with an empty list *)
  in
  (* let results =
    Domainslib.Task.run pool (fun () ->
        Domainslib.Task.parallel_for_reduce pool ( + ) 0
          ~chunk_size:(total_files / (4 * num_domains))
          ~start:0 ~finish:(total_files - 1)
          ~body:(fun _i -> 1))
  in *)
  Domainslib.Task.teardown_pool pool;
  [ ("dummy", results) ]

(* Main program *)
let () =
  if Array.length Sys.argv < 3 then (
    Printf.printf "Usage: %s <directory> <chunk_size> [num_domains]\n"
      Sys.argv.(0);
    exit 1);

  let root_dir = Sys.argv.(1) in
  let chunk_size = int_of_string Sys.argv.(2) in
  let num_domains =
    if Array.length Sys.argv > 3 then int_of_string Sys.argv.(3) else 4
  in

  if not (Sys.file_exists root_dir && Sys.is_directory root_dir) then (
    Printf.eprintf "Error: '%s' is not a valid directory.\n" root_dir;
    exit 1);

  (* Step 1: Collect files into a map *)
  let file_map = collect_files_as_map root_dir StringMap.empty in

  (* Step 2: Parallelize newline counting *)
  let results = parallel_count_newlines file_map chunk_size num_domains in

  (* Step 3: Print results *)
  List.iter
    (fun (path, count) -> Printf.printf "File: %s, Newlines: %d\n" path count)
    results
