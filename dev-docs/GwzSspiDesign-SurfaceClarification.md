Yes. Explicitly defining Negotiate as permitting Kerberos or NTLM, excluding Kerberos-only use, and exposing typed native Selected/Unresolved mechanism observation resolves the counterexample without a policy knob.

Document that callers accept either mechanism before starting; permit unresolved intermediate tokens under that choice; require authoritative mechanism identity on Complete or fail. This addresses P2-1’s missing identity contract. No revised verdict until the correction is settled.
