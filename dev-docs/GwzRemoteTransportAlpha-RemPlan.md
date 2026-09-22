# Alpha correction 1

State P2-1: narrow CLI transport selection. Snapshot never opens a transport
runtime. Tags open one only for fetch/push, or list/delete with a remote.
Closure: actual binary local create/list/delete and snapshot succeed despite
invalid proxy and missing CA file; all four remote tag variants reject invalid
proxy through the new endpoint. Original counterexample recorded before correction.
No core or protocol change; one bounded routing correction. Retained State must
verify closure. Code prior GO is retained subject to changed-range confirmation.
