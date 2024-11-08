TO STORE TOKEN:
git config --global credential.helper cache
or 
git config --global credential.helper store


COMMON COMMANDS
Check Your Current Branch = git branch
Create a New Branch = git checkout -b new-branch-name    (this automatically switches to that branch)
Switch Between Branches = git checkout branch-name
List All Branches = git branch -a

Stage Changes for Commit = git add .
Commit Changes = git commit -m "Your commit message here"
IMPORTANT - dont do git push (this will push to main instead of the branch!!)
To push your committed changes to the remote repository = git push origin branch-name 
Pull Changes from the Remote Repository = git pull origin branch-name

