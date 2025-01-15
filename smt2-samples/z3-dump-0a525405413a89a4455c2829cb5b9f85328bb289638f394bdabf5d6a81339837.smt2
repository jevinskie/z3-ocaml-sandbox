; argc: 4
; argv[0]: z3
; argv[1]: -t
; argv[2]: -T
; argv[3]: /var/folder/FILE
(push)
(declare-const v1 Int)
(declare-const v0 Int)
(declare-const v3 Int)
(declare-const v7 Int)
(declare-const v5 Int)
(declare-const v10 Int)
(declare-const v2 Int)
(declare-const v4 Int)
(declare-const v8 Int)
(declare-const v6 Int)
(declare-const v11 Int)
(declare-const v9 Int)

(assert (and (and (and (and (and (and (and (and (and (and (and (and (and (not (= v9 2)) (= v1 512)) (= v11 v10)) (and (<= -32 v10) (< v10 32))) (= v9 2)) (= v8 v7)) (and (<= 0 v7) (< v7 4))) (= v6 v5)) (and (<= 0 v5) (< v5 32))) (= v4 v3)) (and (<= 0 v3) (< v3 8))) (= v2 32)) (= v1 v0)) (or (= v0 128) (= v0 256) (= v0 512) (= v0 1024) (= v0 2048))))
(check-sat)
(pop)
