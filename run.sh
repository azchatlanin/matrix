export PROJECT_NAME=matrix
export PROJECT_BUILD=examples
export BUILD_VERSION=0.9.$(date +%s)

clear

## BASH_MATIC
BASH_MATIC="${HOME}/.config/bashmatic"
[[ -s ${BASH_MATIC}/init.sh ]] || {
  rm -rf "${BASH_MATIC}" 2>/dev/null
  git clone https://github.com/kigster/bashmatic "${BASH_MATIC}" 2>&1 1>/dev/null
}
source "${BASH_MATIC}/init.sh"

divider.header() {
  h1.purple "Description: Matrix project"
  IFS="|" read -r -a g_plus_plus_info <<< "$(g++ --version 2>&1 | tr '\n' '|')"
  h1 "${bldylw}G++" "${g_plus_plus_info[1]}" "${g_plus_plus_info[2]}" "${g_plus_plus_info[3]}" "${g_plus_plus_info[4]}"
  h1 "${bldylw}GIT:    ${bldblu}$(git --version)"
}

divider.build() {
  hl.subtle "Build project"
  run "bazel build $PROJECT_BUILD"
}

divider.run() {
  hl.subtle "Run project"
  run.set-all show-output-on
  run "bazel run $PROJECT_BUILD"
}

main() {
  divider.header
  divider.build
  divider.run
}
main
