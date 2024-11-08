### TO STORE TOKEN: <br />

<span style="color: pink;">git config --global credential.helper cache</span><br />
or <br />
<span style="color: pink;">git config --global credential.helper store</span><br />
<br />
<br />
### BRANCHING <br />

Check Your Current Branch = <span style="color: pink;">git branch</span><br />
Create a New Branch = <span style="color: pink;">git checkout -b new-branch-name</span> (this automatically switches to that branch)<br />
Switch Between Branches = <span style="color: pink;">git checkout branch-name</span><br />
List All Branches = <span style="color: pink;">git branch -a</span><br />
Make current branch the same as main git rebase origin/main<br />
<br />
### PUSHING <br />
Stage Changes for Commit = <span style="color: pink;">git add .</span><br />
Commit Changes = <span style="color: pink;">git commit -m "Your commit message here"</span><br />
IMPORTANT - don't do <span style="color: pink;">git push</span> (this will push to `main` instead of the branch!!)<br />
To push your committed changes to the remote repository = <span style="color: pink;">git push origin branch-name</span><br />
<br />
### PULLING <br />
Pull Changes = <span style="color: pink;">git pull</span><br />
<br />
### TO DISCARD CHANGES YOU MADE <br />
<span style="color: pink;">git restore .</span><br />
<br />
### STATUS <br />
<span style="color: pink;">git status</span><br />
This should show you the status of your working directory (whether there are uncommitted changes, untracked files, etc.).<br />
Discard untracked files = <span style="color: pink;">git clean -f</span><br />
