#!/usr/bin/env bash
set -euo pipefail

repo="${MIGRAPHX_PR_REPO:-ROCm/AMDMIGraphX}"

if (($# > 0)); then
    prs=("$@")
else
    prs=(4707 4708 4709 4710)
fi

for tool in curl jq; do
    if ! command -v "${tool}" >/dev/null 2>&1; then
        echo "error: ${tool} is required" >&2
        exit 1
    fi
done

api_get() {
    local url=$1
    local -a headers=(-H "Accept: application/vnd.github+json")

    if [[ -n "${GITHUB_TOKEN:-}" ]]; then
        headers+=(-H "Authorization: Bearer ${GITHUB_TOKEN}")
    fi

    curl -fsSL "${headers[@]}" "${url}"
}

for pr in "${prs[@]}"; do
    pr_json=$(api_get "https://api.github.com/repos/${repo}/pulls/${pr}")
    title=$(jq -r '.title' <<<"${pr_json}")
    state=$(jq -r '.state' <<<"${pr_json}")
    draft=$(jq -r '.draft' <<<"${pr_json}")
    mergeable=$(jq -r '.mergeable' <<<"${pr_json}")
    mergeable_state=$(jq -r '.mergeable_state' <<<"${pr_json}")
    html_url=$(jq -r '.html_url' <<<"${pr_json}")
    sha=$(jq -r '.head.sha' <<<"${pr_json}")
    head_ref=$(jq -r '.head.label' <<<"${pr_json}")
    base_ref=$(jq -r '.base.label' <<<"${pr_json}")

    echo "PR #${pr}: ${title}"
    echo "  url: ${html_url}"
    echo "  branch: ${head_ref} -> ${base_ref}"
    echo "  state: ${state} | draft: ${draft} | mergeable: ${mergeable} | mergeable_state: ${mergeable_state}"

    checks_json=$(api_get "https://api.github.com/repos/${repo}/commits/${sha}/check-runs")
    failures=$(jq -r '
        .check_runs[]
        | select((.conclusion != "success") and (.conclusion != "neutral") and (.conclusion != "skipped"))
        | "    - \(.name): status=\(.status) conclusion=\(.conclusion // "pending")"
    ' <<<"${checks_json}")

    echo "  non-passing checks:"
    if [[ -z "${failures}" ]]; then
        echo "    - none"
    else
        echo "${failures}"
    fi

    reviews_json=$(api_get "https://api.github.com/repos/${repo}/pulls/${pr}/reviews")
    review_summary=$(jq -r '
        if length == 0 then
            "    - none"
        else
            group_by(.state)[]
            | "    - \(.[0].state): \(length)"
        end
    ' <<<"${reviews_json}")

    echo "  reviews:"
    echo "${review_summary}"
    echo
done
