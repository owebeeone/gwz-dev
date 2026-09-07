import json, os, pathlib, shutil, subprocess, time, statistics
home=pathlib.Path.home(); out=home/'git/gwz-fs-comparison-20260907'
out.mkdir(exist_ok=False)
helper='/usr/local/sbin/gwz-fs-bench'; gwz=str(home/'git/gwz-pi-build-20260907/debug/gwz'); source=home/'git/gwz-pi-acceptance-20260907'; mount=pathlib.Path('/var/lib/gwz-fs-bench-1000/mnt')
def run(args):
 p=subprocess.run(args,text=True,capture_output=True)
 if p.returncode: raise RuntimeError(str(args)+'\n'+p.stdout+p.stderr)
 return p.stdout
def status(): return json.loads(run([helper,'status']))
assert not status()['image_exists']
results=[]
for idx,fs in enumerate(['xfs','ext4','ext4','xfs']):
 print('START',idx,fs,flush=True)
 run([helper,'create',fs]); empty=status()
 shutil.copytree(source,mount/'source',symlinks=True)
 populated=status(); times=[]
 for n in range(10):
  start=time.perf_counter()
  result=run([gwz,'--root',str(mount/'source'),'local','clone',f'bench{n}',str(mount/f'lane{n}'),'--json'])
  elapsed=time.perf_counter()-start; times.append(elapsed)
  (out/f'{idx}-{fs}-clone{n}.json').write_text(result)
  print(fs,n,round(elapsed,3),flush=True)
 after=status()
 row=dict(round=idx,filesystem=fs,empty=empty,source=populated,after_clones=after,clone_seconds=times,median_seconds=statistics.median(times),total_seconds=sum(times))
 results.append(row); (out/'results.json').write_text(json.dumps(results,indent=2))
 run([helper,'destroy']); assert not status()['image_exists']
 print('DONE',fs,'median',row['median_seconds'],'allocated growth',after['image_allocated_bytes']-populated['image_allocated_bytes'],flush=True)
print('RESULTS',out,flush=True)
