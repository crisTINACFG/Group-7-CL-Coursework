</pre>
###TO STORE TOKEN:<br />
git config --global credential.helper cache<br />
or <br />
git config --global credential.helper store<br />
<br />
<br />
###BRANCHING<br />
Check Your Current Branch = git branch<br />
Create a New Branch = git checkout -b new-branch-name    (this automatically switches to that branch)<br />
Switch Between Branches = git checkout branch-name<br />
List All Branches = git branch -a<br />
<br />
###PUSHING<br />
Stage Changes for Commit = git add .<br />
Commit Changes = git commit -m "Your commit message here"<br />
IMPORTANT - dont do git push (this will push to main instead of the branch!!)<br />
To push your committed changes to the remote repository = git push origin branch-name <br />
<br />
###PULLING<br />
Pull Changes = git pull<br />
<br />
###TO DISCARD CHANGES YOU MADE<br />
git restore .<br />
<br />
###STATUS<br />
git status<br />
This should show you the status of your working directory (whether there are uncommitted changes, untracked files, etc.).<br />
discard untracked files = git clean -f<br />
</pre>
