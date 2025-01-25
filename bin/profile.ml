(****************************************************************************)
(*     Sail                                                                 *)
(*                                                                          *)
(*  Sail and the Sail architecture models here, comprising all files and    *)
(*  directories except the ASL-derived Sail code in the aarch64 directory,  *)
(*  are subject to the BSD two-clause licence below.                        *)
(*                                                                          *)
(*  The ASL derived parts of the ARMv8.3 specification in                   *)
(*  aarch64/no_vector and aarch64/full are copyright ARM Ltd.               *)
(*                                                                          *)
(*  Copyright (c) 2013-2021                                                 *)
(*    Kathyrn Gray                                                          *)
(*    Shaked Flur                                                           *)
(*    Stephen Kell                                                          *)
(*    Gabriel Kerneis                                                       *)
(*    Robert Norton-Wright                                                  *)
(*    Christopher Pulte                                                     *)
(*    Peter Sewell                                                          *)
(*    Alasdair Armstrong                                                    *)
(*    Brian Campbell                                                        *)
(*    Thomas Bauereiss                                                      *)
(*    Anthony Fox                                                           *)
(*    Jon French                                                            *)
(*    Dominic Mulligan                                                      *)
(*    Stephen Kell                                                          *)
(*    Mark Wassell                                                          *)
(*    Alastair Reid (Arm Ltd)                                               *)
(*                                                                          *)
(*  All rights reserved.                                                    *)
(*                                                                          *)
(*  This work was partially supported by EPSRC grant EP/K008528/1 <a        *)
(*  href="http://www.cl.cam.ac.uk/users/pes20/rems">REMS: Rigorous          *)
(*  Engineering for Mainstream Systems</a>, an ARM iCASE award, EPSRC IAA   *)
(*  KTF funding, and donations from Arm.  This project has received         *)
(*  funding from the European Research Council (ERC) under the European     *)
(*  Union’s Horizon 2020 research and innovation programme (grant           *)
(*  agreement No 789108, ELVER).                                            *)
(*                                                                          *)
(*  This software was developed by SRI International and the University of  *)
(*  Cambridge Computer Laboratory (Department of Computer Science and       *)
(*  Technology) under DARPA/AFRL contracts FA8650-18-C-7809 ("CIFV")        *)
(*  and FA8750-10-C-0237 ("CTSRD").                                         *)
(*                                                                          *)
(*  SPDX-License-Identifier: BSD-2-Clause                                   *)
(****************************************************************************)

let opt_profile = ref true

type profile = {
  smt_calls : int Atomic.t;
  smt_utime : float Atomic.t;
  smt_stime : float Atomic.t;
  smt_cutime : float Atomic.t;
  smt_cstime : float Atomic.t;
  smt_wtime : float Atomic.t;
}

let new_profile =
  {
    smt_calls = Atomic.make 0;
    smt_utime = Atomic.make 0.0;
    smt_stime = Atomic.make 0.0;
    smt_cutime = Atomic.make 0.0;
    smt_cstime = Atomic.make 0.0;
    smt_wtime = Atomic.make 0.0;
  }

let profile_stack = ref []

let update_profile f =
  match !profile_stack with [] -> () | p :: ps -> profile_stack := f p :: ps

let start_smt () =
  update_profile (fun p ->
      Atomic.incr p.smt_calls;
      { p with smt_calls = p.smt_calls });
  (Unix.times (), Unix.gettimeofday ())

let finish_smt ((t : Unix.process_times), (wt : float)) =
  update_profile (fun p ->
      let ut, st, cut, cst =
        (t.tms_utime, t.tms_stime, t.tms_cutime, t.tms_cstime)
      in
      let nwt = Unix.gettimeofday () in
      let nt = Unix.times () in
      let nut, nst, ncut, ncst =
        (nt.tms_utime, nt.tms_stime, nt.tms_cutime, nt.tms_cstime)
      in
      let dut, dst, dcut, dcst, dwt =
        (nut -. ut, nst -. st, ncut -. cut, ncst -. cst, nwt -. wt)
      in
      let quit_loop = ref false in
      while not !quit_loop do
        let old_t = Atomic.get p.smt_utime in
        let new_t = old_t +. dut in
        quit_loop := Atomic.compare_and_set p.smt_utime old_t new_t
      done;
      quit_loop := false;
      while not !quit_loop do
        let old_t = Atomic.get p.smt_stime in
        let new_t = old_t +. dst in
        quit_loop := Atomic.compare_and_set p.smt_stime old_t new_t
      done;
      quit_loop := false;
      while not !quit_loop do
        let old_t = Atomic.get p.smt_cutime in
        let new_t = old_t +. dcut in
        quit_loop := Atomic.compare_and_set p.smt_cutime old_t new_t
      done;
      quit_loop := false;
      while not !quit_loop do
        let old_t = Atomic.get p.smt_cstime in
        let new_t = old_t +. dcst in
        quit_loop := Atomic.compare_and_set p.smt_cstime old_t new_t
      done;
      quit_loop := false;
      while not !quit_loop do
        let old_t = Atomic.get p.smt_wtime in
        let new_t = old_t +. dwt in
        quit_loop := Atomic.compare_and_set p.smt_wtime old_t new_t
      done;
      p)

let start () =
  profile_stack := new_profile :: !profile_stack;
  Unix.gettimeofday ()

let finish (msg : string) (wt : float) =
  let open Printf in
  if !opt_profile then
    let depth = 2 * (List.length !profile_stack - 1) in
    match !profile_stack with
    | p :: ps ->
        let indent =
          if depth > 0 then
            String.init depth (fun i -> if i land 1 = 0 then '|' else ' ')
          else ""
        in
        (* Note ksprintf prerr_endline flushes unlike eprintf so the profiling output occurs immediately *)
        ksprintf prerr_endline "%s%s %s: %f s" indent "Profiled" msg
          (Unix.gettimeofday () -. wt);
        ksprintf prerr_endline
          "%s  SMT calls: %d, SMT utime: %f s stime: %f s cutime: %f s cstime: \
           %f s wtime: %f s"
          indent (Atomic.get p.smt_calls) (Atomic.get p.smt_utime)
          (Atomic.get p.smt_stime) (Atomic.get p.smt_cutime)
          (Atomic.get p.smt_cstime) (Atomic.get p.smt_wtime);
        profile_stack := ps
    | [] -> ()
