</pre>
###TO STORE TOKEN:
git config --global credential.helper cache
or 
git config --global credential.helper store


###BRANCHING
Check Your Current Branch = git branch
Create a New Branch = git checkout -b new-branch-name    (this automatically switches to that branch)
Switch Between Branches = git checkout branch-name
List All Branches = git branch -a

###PUSHING
Stage Changes for Commit = git add .
Commit Changes = git commit -m "Your commit message here"
IMPORTANT - dont do git push (this will push to main instead of the branch!!)
To push your committed changes to the remote repository = git push origin branch-name 

###PULLING
Pull Changes = git pull

###TO DISCARD CHANGES YOU MADE
git restore .

###STATUS
git status
This should show you the status of your working directory (whether there are uncommitted changes, untracked files, etc.).
discard untracked files = git clean -f
</pre>
