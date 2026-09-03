<!-- HN post draft (Gianni's voice, fact-checked). Plain-text friendly: bare URLs, no markdown links. -->

Show HN: GWZ – Git Workspace Zone (multi-repo that feels like plain Git)

Coming from the land of the mono-repo (ex-googler), I decided to embrace the
multi-repo, plain-git style. Methought, hey, submodules, what could possibly
go wrong. Well, how about everything: detached HEADs, impossibly tedious
commit management, and just about everything else. "OKAY" I said, a simple
Python script should be fine, right? Well, NO. It turns out to be more
complex. So I thought "others have got this, right? My time in mono-repo land
was long and someone will have solved this" — well, yeah NAH. I won't bore
you (the longer, fairer comparison is in the docs) but everything I tried had
baggage. All I want is a tool that gets out of my way and makes my multi-repo
look like a mono-repo when I want, and a multi-repo when I want. I want my
cake and I want it now.

"claude and codex, make me gwz" I said, "and make it work much like using git
itself" I said — which they proceeded to ignore, but my many-layered
pleadings got them to see the merits of my ways.

The similarities don't end at gwz being 3 letters starting with "g". I wanted
the gwz workflow to feel like a git workflow:

- Member repositories remain ordinary Git repositories. No submodules
  unexpectedly detaching (unless you ask to materialize to a tag/snapshot).
- Snapshots and commit markers can recover workspace state. That state lives
  in the root repo.
- gwz status, diff, add, pull, push, commit, tag, stash, clone, init all play
  similar (not exact) roles to their git namesakes.
- Extra commands to manage members: gwz repo create / clone / add / sync /
  detach / attach.
- A forall command that runs anything across selected member repos.

Also, the member repos are hidden from the root repo's git (via
.git/info/exclude), so you can't mess up the members from the root.

For my daily workflow gwz is great, and it's now my goto — I hardly use raw
git anymore. But gwz, like any form of art, will never be finished. There's
always more; it's time to unveil anyway.

Caveats: it's written in Rust and I can't yet write Rust if my life depended
on it. So why Rust and not Python (my usual goto)? I have another project
that needs to manage repos, which meant gwz had to be carved up into a CLI
and a "core" crate with a typed protocol between them — so the engine is
embeddable and wire-friendly, not fused to a terminal. That other project's
target is also Rust, and (suspense) it's not ready to reveal, so: stay tuned.

The little LLM helpers created copious amounts of very fine documentation,
which I invite you to look at: https://owebeeone.github.io/gwz-cli/

Honest state:

- It has a few rough UX edges, nothing so egregious it'll make you chunder.
- For my workflow gwz is great. YMMV.
- The CLI comes in Rust and Python variants. I use the Rust gwz daily; the
  Python gwz-py should work but gets less use, so it probably has bugs — it
  started life as a validation harness proving the core's message protocol
  holds up when driven from a second language.
- It's v0.9.2 and pre-1.0: the protocol and some command surfaces may still
  move.
- GPL-2.0-only, the same license family as git.

What I'd like from you: point it at your own multi-repo pile (gwz init, then
gwz repo add each checkout — members aren't modified) and tell me where the
model doesn't fit; the first missing command that would stop you adopting it;
and whether the member lifecycle rules (detach/attach with recorded commit
evidence) read as reassuring or as bureaucracy.

Code: https://github.com/owebeeone/gwz-cli (CLI),
https://github.com/owebeeone/gwz-core (engine),
https://github.com/owebeeone/gwz-py (Python, pip install gwz).
Docs: https://owebeeone.github.io/gwz-cli/
Issues: https://github.com/owebeeone/gwz-cli/issues
