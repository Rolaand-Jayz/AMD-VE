.. meta::
   :description: MIGraphX pull request remediation workflow
   :keywords: MIGraphX, pull requests, reviews, CI, merge conflicts

====================================
Pull Request Remediation Workflow
====================================

Use this workflow when a group of MIGraphX pull requests needs a current
mergeability, review, and CI status check before remediation.

Status helper
=============

Run the local helper from the repository root:

.. code-block:: bash

   tools/check_migraphx_prs.sh

By default it checks PRs 4707 through 4710 on ``ROCm/AMDMIGraphX``. Pass PR
numbers to check a different set:

.. code-block:: bash

   tools/check_migraphx_prs.sh 4707 4709

Set ``MIGRAPHX_PR_REPO`` to inspect another fork and ``GITHUB_TOKEN`` to avoid
anonymous GitHub API rate limits:

.. code-block:: bash

   MIGRAPHX_PR_REPO=ROCm/AMDMIGraphX GITHUB_TOKEN=... tools/check_migraphx_prs.sh

The script reports:

* PR title, URL, head branch, and base branch
* GitHub mergeability and ``mergeable_state``
* non-passing check runs for the head commit
* grouped review states

PRs 4707 through 4710
====================

As of 2026-04-18, the local status check reported:

* PR 4707: ``mergeable_state=dirty`` with one ``CHANGES_REQUESTED`` review
* PR 4708: ``mergeable_state=dirty`` with no failing checks reported
* PR 4709: ``mergeable_state=unstable`` with ``cppcheck``, ``format``, and
  ``licensing`` failures, plus one ``CHANGES_REQUESTED`` review
* PR 4710: ``mergeable_state=dirty`` with one ``CHANGES_REQUESTED`` review

Remediation order
=================

1. Start with PRs that have ``CHANGES_REQUESTED`` because their final patch may
   change after review feedback is addressed.
2. Resolve ``dirty`` branches against current ``develop`` before rerunning CI.
3. For PRs with failing checks, fix local style and stamped-file issues first,
   then run the heavier build checks.
4. Re-run the status helper after each pushed update to confirm GitHub has the
   expected head SHA and check results.

Local repair workflow
=====================

Use a clean branch based on current upstream ``develop`` when the PR branch is
old or conflict-heavy:

.. code-block:: bash

   git remote add upstream https://github.com/ROCm/AMDMIGraphX.git
   git fetch upstream

   git checkout -b fix/pr-4709 upstream/develop
   # Re-apply the intended PR changes with cherry-pick or a focused patch.

Run the local checks that correspond to the failing GitHub jobs:

.. code-block:: bash

   python3 tools/format.py upstream/develop
   python3 tools/check_stamped.py upstream/develop

   cmake -S . -B build -DCPPCHECK_BUILD_DIR=cppcheck-cache -DBUILD_DEV=On
   cmake --build build --target cppcheck --parallel "$(nproc)"
   tools/cppcheck/test.sh

If a branch cannot be repaired cleanly, recreate it from ``upstream/develop``
and apply only the intended commits. Avoid carrying unrelated merge conflict
resolutions forward.
