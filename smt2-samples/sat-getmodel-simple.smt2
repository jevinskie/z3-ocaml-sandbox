(declare-const x Int)
(assert (= x 42))
(check-sat)
(get-model)
