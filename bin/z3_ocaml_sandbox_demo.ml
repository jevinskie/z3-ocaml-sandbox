(* open Z3_mini

let () =
  let _ = z3_mini_ctx in
  print_endline "done" *)
module StringMap = Hashtbl.Make (String)

module StringHtbl = Hashtbl.Make (struct
  type t = string

  let equal = String.equal
  let hash = String.hash
end)

open Domainslib
open Z3_mini

(* Function to read file contents *)
let read_file path =
  let ic = open_in path in
  let len = in_channel_length ic in
  let contents = really_input_string ic len in
  close_in ic;
  contents

(* Recursively walk through a directory and build a map of file paths to contents *)
let rec collect_files_as_map dir tbl =
  Sys.readdir dir |> Array.to_list
  |> List.iter (fun entry ->
         let path = Filename.concat dir entry in
         if Sys.is_directory path then collect_files_as_map path tbl
         else if Filename.check_suffix path ".smt2" then
           let content = read_file path in
           StringHtbl.add tbl path content)

(* Count the number of newlines in a string *)
let count_newlines content =
  String.fold_left (fun acc c -> if c = '\n' then acc + 1 else acc) 0 content

(* Parallel newline counting with configurable chunk size *)
let parallel_count_newlines file_map chunk_size num_domains =
  let total_files = StringHtbl.length file_map in
  let pool = Domainslib.Task.setup_pool ~num_domains () in

  (* Convert file_map to an array for indexed access *)
  let files = StringHtbl.to_seq_keys file_map |> Array.of_seq in

  (* Use parallel_for_reduce to process files in parallel *)
  let results =
    Domainslib.Task.run pool (fun () ->
        Domainslib.Task.parallel_for_reduce pool ( @ ) [] ~chunk_size ~start:0
          ~finish:(total_files - 1) ~body:(fun i ->
            let key = Array.get files i in
            let content = StringHtbl.find file_map key in
            [ (key, count_newlines content) ]))
  in
  Domainslib.Task.teardown_pool pool;
  results

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
  let htab = StringHtbl.create 246189 in
  let () = collect_files_as_map root_dir htab in
  let file_map = htab in

  (* Step 2: Parallelize newline counting *)
  let results = parallel_count_newlines file_map chunk_size num_domains in

  (* Step 3: Print results *)
  let _, counts = List.split results in
  let sum = List.fold_left ( + ) 0 counts in
  Printf.printf "Newlines: %d\n" sum

let () =
  let ctx = Z3_mini_ctx.mk 1 in
  let smt2_sat = "(declare-const x Int) (assert (= x 42))" in
  let sat = Z3_mini_ctx.check_sat ctx smt2_sat in
  let () = Printf.printf "smt2 sat: %d\nsmt2:%s\n" sat smt2_sat in
  let model = Z3_mini_ctx.get_model_new ctx smt2_sat in
  let () = Printf.printf "smt2 model: %s\n" model in
  let smt2_unsat =
    "(declare-const x Int) (assert (<= x 42)) (assert (>= x 243))"
  in
  let unsat = Z3_mini_ctx.check_sat ctx smt2_unsat in
  let () = Printf.printf "smt2 unsat: %d\nsmt2:%s\n" unsat smt2_unsat in
  Z3_mini_ctx.del ctx
