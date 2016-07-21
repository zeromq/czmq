# Pull Request Notice

Before sending a pull request make sure each commit solves one clear, minimal,
plausible problem. Further each commit should have the following format:

```
Problem: X is broken

Solution: do Y and Z to fix X
```

Please avoid sending a pull request with recursive merge nodes, as they
are impossible to fix once merged. Please rebase your branch on
zeromq/czmq master instead of merging it.

git remote add upstream git@github.com:zeromq/czmq.git
git fetch upstream
git rebase upstream/master
git push -f

If you are new please have a look at our contributing guidelines:
[CONTRIBUTING.md](https://github.com/zeromq/czmq/blob/master/CONTRIBUTING.md)
