open Domainslib
open Z3_mini
module Z3 = Z3_mini_ctx

module Hashtbl = struct
  include Base.Hashtbl
end

module Sexp = Sexplib.Sexp

module FancyString = struct
  type t = string

  let compare = String.compare
  let equal = String.equal
  let hash = String.hash
end

module FileMappingPath = struct
  include FancyString

  let sexp_of_t t =
    let a x = Sexp.Atom x and l x = Sexp.List x in
    l [ a "path"; a t ]
end

module FileMappingContent = struct
  include FancyString

  let sexp_of_t t =
    let a x = Sexp.Atom x and l x = Sexp.List x in
    l [ a "content"; a t ]
end

module FileMap = struct
  type t = (FileMappingPath.t, FileMappingContent.t) Hashtbl.t

  let pp ppf values =
    Hashtbl.iteri values ~f:(fun ~key ~data ->
        Format.fprintf ppf "@[<1>%s: %s@]@." key data)
end

module FileSmtMap = struct
  type t = (FileMappingPath.t, Z3_mini_ctx.lbool) Hashtbl.t

  let pp ppf values =
    Hashtbl.iteri values ~f:(fun ~key ~data ->
        Format.fprintf ppf "@[<1>%s: %a@]@." key Z3.pp_lbool data)
end

let pp_path_content_pair ppf (path, content) =
  Format.fprintf ppf "@[path: \"%s\" content: \"%s\"@]@." path content

let pp_path_content_list ppf path_content_list =
  path_content_list
  |> List.iteri (fun i path_content_pair ->
         Format.fprintf ppf "@[%d: %a@]@." i pp_path_content_pair
           path_content_pair)

(* let dls_make_key do_model = Domain.DLS.new_key (fun () -> Z3.mk do_model) *)
let dls_make_key = Domain.DLS.new_key (fun () -> Z3.mk false)

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
           Hashtbl.set tbl ~key:path ~data:content)

(* Count the number of newlines in a string *)
let count_newlines content =
  String.fold_left (fun acc c -> if c = '\n' then acc + 1 else acc) 0 content

(* Parallel newline counting with configurable chunk size *)
let parallel_count_newlines file_map chunk_size num_domains =
  let total_files = Hashtbl.length file_map in
  let pool = Domainslib.Task.setup_pool ~num_domains () in

  (* Convert file_map to an array for indexed access *)
  let files = Hashtbl.keys file_map |> Array.of_list in

  (* Use parallel_for_reduce to process files in parallel *)
  let results =
    Domainslib.Task.run pool (fun () ->
        Domainslib.Task.parallel_for_reduce pool ( @ ) [] ~chunk_size ~start:0
          ~finish:(total_files - 1) ~body:(fun i ->
            let key = Array.get files i in
            let content = Hashtbl.find_exn file_map key in
            [ (key, count_newlines content) ]))
  in
  Domainslib.Task.teardown_pool pool;
  results

let baby_test () =
  let ctx = Z3.mk true in
  let smt2_sat = "(declare-const x Int) (assert (= x 42))" in
  let sat = Z3.check_sat ctx smt2_sat in
  Printf.printf "smt2 sat: %s\nsmt2:%s\n" (Z3.lbool_to_string sat) smt2_sat;
  let model = Z3.get_model ctx smt2_sat in
  Printf.printf "smt2 model: %s\n%!" model;
  let smt2_unsat =
    "(declare-const x Int) (assert (<= x 42)) (assert (>= x 243))"
  in
  let unsat = Z3.check_sat ctx smt2_unsat in
  Printf.printf "smt2 unsat: %s\nsmt2:%s\n" (Z3.lbool_to_string unsat)
    smt2_unsat;
  Z3.del ctx

(* let () = baby_test *)

let do_test ctx =
  let smt2_sat = "(declare-const x Int) (assert (= x 42))" in
  let sat = Z3.check_sat ctx smt2_sat in
  Format.printf "smt2 sat: %a\nsmt2:%s\n" Z3.pp_lbool sat smt2_sat;
  let model = Z3.get_model ctx smt2_sat in
  Format.printf "smt2 model: %s\n%!" model;
  let smt2_unsat =
    "(declare-const x Int) (assert (<= x 42)) (assert (>= x 243))"
  in
  let unsat = Z3.check_sat ctx smt2_unsat in
  Format.printf "smt2 unsat: %a\nsmt2:%s\n" Z3.pp_lbool unsat smt2_unsat

(* let () = Z3.with_z3_context true do_test *)

let z3_mini_parse_test file_map chunk_size num_domains =
  Format.printf
    "Z3_mini checking SMT sat for %d programs with chunk_size: %d and \
     num_domins: %d\n"
    (Hashtbl.length file_map) chunk_size num_domains;
  let total_files = Hashtbl.length file_map in
  let pool = Domainslib.Task.setup_pool ~name:"z3-check-sat" ~num_domains () in

  (* prep inputs *)
  let blobs_list = Hashtbl.data file_map in
  let blobs = blobs_list |> Array.of_list in
  let num_smt2 = Array.length blobs in
  let res = Array.make num_smt2 Z3.Uninit in

  (* Use parallel_for to process files in parallel *)
  Domainslib.Task.run pool (fun () ->
      Domainslib.Task.parallel_for pool ~chunk_size ~start:0
        ~finish:(total_files - 1) ~body:(fun i ->
          let ctx = Domain.DLS.get dls_make_key in
          let blob = Array.get blobs i in
          let sat = Z3.check_sat ctx blob in
          res.(i) <- sat));
  Domainslib.Task.teardown_pool pool;
  Format.printf "num smt2: %d\n" num_smt2;
  let num_sat =
    Array.fold_left
      (fun acc a -> match a with Z3.True -> acc + 1 | _ -> acc)
      0 res
  in
  Format.printf "num  sat: %d\n" num_sat;
  Format.printf "done\n"

let z3_subproc_shell_parse_test file_map chunk_size num_domains =
  Format.printf
    "Z3 subproc shell checking SMT sat for %d programs with chunk_size: %d and \
     num_domins: %d\n"
    (Hashtbl.length file_map) chunk_size num_domains;
  let total_files = Hashtbl.length file_map in
  let pool = Domainslib.Task.setup_pool ~name:"z3-check-sat" ~num_domains () in

  (* prep inputs *)
  let blobs_list = Hashtbl.data file_map in
  let blobs = blobs_list |> Array.of_list in
  let num_smt2 = Array.length blobs in
  let res = Array.make num_smt2 Z3.Uninit in

  (* Use parallel_for to process files in parallel *)
  Domainslib.Task.run pool (fun () ->
      Domainslib.Task.parallel_for pool ~chunk_size ~start:0
        ~finish:(total_files - 1) ~body:(fun i ->
          let ctx = Domain.DLS.get dls_make_key in
          let blob = Array.get blobs i in
          let sat = Z3.check_sat ctx blob in
          res.(i) <- sat));
  Domainslib.Task.teardown_pool pool;
  Format.printf "num smt2: %d\n" num_smt2;
  let num_sat =
    Array.fold_left
      (fun acc a -> match a with Z3.True -> acc + 1 | _ -> acc)
      0 res
  in
  Format.printf "num  sat: %d\n" num_sat;
  Format.printf "done\n"

let z3_subproc_noshell_parse_test file_map chunk_size num_domains =
  Format.printf
    "Z3 subproc noshell checking SMT sat for %d programs with chunk_size: %d \
     and num_domins: %d\n"
    (Hashtbl.length file_map) chunk_size num_domains;
  let total_files = Hashtbl.length file_map in
  let pool = Domainslib.Task.setup_pool ~name:"z3-check-sat" ~num_domains () in

  (* prep inputs *)
  let blobs_list = Hashtbl.data file_map in
  let blobs = blobs_list |> Array.of_list in
  let num_smt2 = Array.length blobs in
  let res = Array.make num_smt2 Z3.Uninit in

  (* Use parallel_for to process files in parallel *)
  Domainslib.Task.run pool (fun () ->
      Domainslib.Task.parallel_for pool ~chunk_size ~start:0
        ~finish:(total_files - 1) ~body:(fun i ->
          let ctx = Domain.DLS.get dls_make_key in
          let blob = Array.get blobs i in
          let sat = Z3.check_sat ctx blob in
          res.(i) <- sat));
  Domainslib.Task.teardown_pool pool;
  Format.printf "num smt2: %d\n" num_smt2;
  let num_sat =
    Array.fold_left
      (fun acc a -> match a with Z3.True -> acc + 1 | _ -> acc)
      0 res
  in
  Format.printf "num  sat: %d\n" num_sat;
  Format.printf "done\n"

(* Main program *)
let main =
  if Array.length Sys.argv < 3 then (
    Printf.printf "Usage: %s <directory> <chunk_size> [num_domains]\n"
      Sys.argv.(0);
    exit 1);

  let root_dir = Sys.argv.(1) in
  let chunk_size = int_of_string Sys.argv.(2) in
  let num_domains =
    if Array.length Sys.argv > 3 then int_of_string Sys.argv.(3)
    else Domain.recommended_domain_count ()
  in

  if not (Sys.file_exists root_dir && Sys.is_directory root_dir) then (
    Printf.eprintf "Error: '%s' is not a valid directory.\n" root_dir;
    exit 1);

  (* Step 1: Collect files into a map *)
  let file_map = Hashtbl.create (module FileMappingPath) in
  let t = Profile.start () in
  collect_files_as_map root_dir file_map;
  Profile.finish "reading input SMT2" t;

  (* Format.printf "file_map: %a\n" FileMap.pp file_map; *)

  (*
  (* Step 2: Parallelize newline counting *)
  let results = parallel_count_newlines file_map chunk_size num_domains in

  (* Step 3: Print results *)
  let _, counts = List.split results in
  let sum = List.fold_left ( + ) 0 counts in
  Printf.printf "Newlines: %d\n" sum;
  *)

  (* Step 4: check SMT *)
  let t = Profile.start () in
  z3_mini_parse_test file_map chunk_size num_domains;
  Profile.finish "Parallel Z3_mini FFI test" t;

  let t = Profile.start () in
  z3_subproc_shell_parse_test file_map chunk_size num_domains;
  Profile.finish "Z3 subproc shell test" t;

  let t = Profile.start () in
  z3_subproc_noshell_parse_test file_map chunk_size num_domains;
  Profile.finish "Z3 subproc noshell test" t

let () = main
