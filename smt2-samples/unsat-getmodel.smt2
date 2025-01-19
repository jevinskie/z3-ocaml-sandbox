; argc: 4
; argv[0]: z3
; argv[1]: -t
; argv[2]: -T
; argv[3]: /var/folder/FILE
(set-option :produce-unsat-cores true)
(set-option :produce-models true)
(declare-const v0 Int)
(declare-const v12 Int)
(declare-const v10 Int)
(declare-const v6 Int)
(declare-const v8 Int)
(declare-const v2 Int)
(declare-const v4 Int)
(declare-const v1 Int)
(declare-const v13 Int)
(declare-const v11 Int)
(declare-const v7 Int)
(declare-const v9 Int)
(declare-const v3 Int)
(declare-const v5 Int)

(assert (and (and (and (and (and (and (and (and (and (and (and (and (and (and (not (and (or (not (or (or (= v11 8) (= v11 16)) (= v11 32))) (or (= v11 8) (= v11 16) (= v11 32))) (or (not (or (= v11 8) (= v11 16) (= v11 32))) (or (or (= v11 8) (= v11 16)) (= v11 32))))) (= v13 v12)) (or (= v12 64) (= v12 32))) (= v11 v10)) (or (= v10 8) (= v10 16) (= v10 32) (= v10 64))) (= v9 v8)) (and (<= 0 v8) (< v8 (to_int (^ 2 (+ (- 4 (+ v5 1)) 1)))))) (= v7 v6)) (or (= v6 128) (= v6 64))) (= v5 v4)) (or (= v4 0) (= v4 1) (= v4 2))) (= v3 v2)) (and (<= 0 v2) (< v2 32))) (= v1 v0)) (and (<= 0 v0) (< v0 32))))
(check-sat)
(get-model)
(get-unsat-core)
