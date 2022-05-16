#!/bin/bash
repoName=${PWD##*/}
echo "Pushing into the $repoName repository.."
git push $GIT_REPO_URL/${repoName}.git
