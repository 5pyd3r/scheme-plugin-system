;;; hello_plugin.ss — Demo Scheme plugin for Chez Scheme plugin system

(define result 0)

;; Access the C function registered via Sforeign_symbol.
;; (foreign-procedure "host_multiply" () int) creates a Scheme procedure
;; that calls the C function named "host_multiply" with no arguments,
;; returning an int.
(define host-multiply
  (foreign-procedure "host_multiply" () integer-32))

(define (run-plugin)
  (display "[Scheme] run-plugin: calling host-multiply...\n")
  (set! result (host-multiply))
  (display "[Scheme] run-plugin: result = ")
  (display result)
  (newline))

(define (get-result)
  result)

;; Auto-execute on load
(run-plugin)
