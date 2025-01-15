; argc: 4
; argv[0]: z3
; argv[1]: -t
; argv[2]: -T
; argv[3]: /var/folder/FILE
; (push)
(declare-const v2 Int)
(declare-const v3 Int)
(declare-const v0 Bool)
(declare-const v4 Bool)
(declare-const v5 Int)
(declare-const v1 Int)

(assert (and (and (and (and (and (and (and (not (= (+ (+ 8 (- 56 v3)) v5) 64)) (and (<= 0 (- v3 1)) (< (- v3 1) 64))) (and (<= 0 (- (- 56 v3) 1)) (< (- (- 56 v3) 1) 64))) v4) (not v0)) (not (and v0 (>= v3 55)))) (= v3 v2)) (= v1 (ite v0 55 63))))
; (check-sat)
; (pop)
