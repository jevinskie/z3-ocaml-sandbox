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

type profile = { smt_calls : int Atomic.t; smt_time : float Atomic.t }

let new_profile = { smt_calls = Atomic.make 0; smt_time = Atomic.make 0.0 }
let profile_stack = ref []

let update_profile f =
  match !profile_stack with [] -> () | p :: ps -> profile_stack := f p :: ps

let start_smt () =
  update_profile (fun p ->
      Atomic.incr p.smt_calls;
      { p with smt_calls = p.smt_calls });
  Sys.time ()

let finish_smt t =
  update_profile (fun p ->
      let quit_loop = ref false in
      let new_t = ref Float.nan in
      while not !quit_loop do
        let delta_t = Sys.time () -. t in
        let old_t = Atomic.get p.smt_time in
        new_t := old_t +. delta_t;
        quit_loop := Atomic.compare_and_set p.smt_time old_t !new_t
      done;
      let new_atomic_t = Atomic.make !new_t in
      { p with smt_time = new_atomic_t })

let start () =
  profile_stack := new_profile :: !profile_stack;
  Sys.time ()

let finish msg t =
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
        ksprintf prerr_endline "%s%s %s: %fs" indent "Profiled" msg
          (Sys.time () -. t);
        ksprintf prerr_endline "%s  SMT calls: %d, SMT time: %fs" indent
          (Atomic.get p.smt_calls) (Atomic.get p.smt_time);
        profile_stack := ps
    | [] -> ()
