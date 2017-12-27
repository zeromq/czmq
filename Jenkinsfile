/*
        NOTE: This file was customized after generation; regenerate its
              zproject and merge differences carefully!

    czmq - The high-level C binding for 0MQ

    Copyright (c) the Contributors as noted in the AUTHORS file.
    This file is part of CZMQ, the high-level C binding for 0MQ:
    http://czmq.zeromq.org.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    NOTE : This Jenkins pipeline script only handles the self-testing of your
    project. If you also want the successful codebase published or deployed,
    you can define a helper job - see the reference implementation skeleton at
    https://github.com/zeromq/zproject/blob/master/Jenkinsfile-deploy.example

*/

pipeline {
    agent { label "libzmq4-dev && ( linux || macosx || bsd || solaris || posix || windows )" }
    parameters {
        // Use DEFAULT_DEPLOY_BRANCH_PATTERN and DEFAULT_DEPLOY_JOB_NAME if
        // defined in this jenkins setup -- in Jenkins Management Web-GUI
        // see Configure System / Global properties / Environment variables
        // Default (if unset) is empty => no deployment attempt after good test
        // See zproject Jenkinsfile-deploy.example for an example deploy job.
        // TODO: Try to marry MultiBranchPipeline support with pre-set defaults
        // directly in MultiBranchPipeline plugin, or mechanism like Credentials,
        // or a config file uploaded to master for all jobs or this job, see
        // https://jenkins.io/doc/pipeline/examples/#configfile-provider-plugin
        string (
            defaultValue: '${DEFAULT_DEPLOY_BRANCH_PATTERN}',
            description: 'Regular expression of branch names for which a deploy action would be attempted after a successful build and test; leave empty to not deploy. Reasonable value is ^(master|release/.*|feature/*)$',
            name : 'DEPLOY_BRANCH_PATTERN')
        string (
            defaultValue: '${DEFAULT_DEPLOY_JOB_NAME}',
            description: 'Name of your job that handles deployments and should accept arguments: DEPLOY_GIT_URL DEPLOY_GIT_BRANCH DEPLOY_GIT_COMMIT -- and it is up to that job what to do with this knowledge (e.g. git archive + push to packaging); leave empty to not deploy',
            name : 'DEPLOY_JOB_NAME')
        booleanParam (
            defaultValue: true,
            description: 'If the deployment is done, should THIS job wait for it to complete and include its success or failure as the build result (true), or should it schedule the job and exit quickly to free up the executor (false)',
            name: 'DEPLOY_REPORT_RESULT')
        booleanParam (
            defaultValue: true,
            description: 'Attempt stable build without DRAFT API in this run?',
            name: 'DO_BUILD_WITHOUT_DRAFT_API')
        booleanParam (
            defaultValue: true,
            description: 'Attempt build with DRAFT API in this run?',
            name: 'DO_BUILD_WITH_DRAFT_API')
        booleanParam (
            defaultValue: false,
            description: 'Attempt a build with docs in this run? (Note: corresponding tools are required in the build environment)',
            name: 'DO_BUILD_DOCS')
        booleanParam (
            defaultValue: true,
            description: 'Attempt "make check" in this run?',
            name: 'DO_TEST_CHECK')
        booleanParam (
            defaultValue: true,
            description: 'Attempt "make memcheck" in this run?',
            name: 'DO_TEST_MEMCHECK')
        booleanParam (
            defaultValue: true,
            description: 'Attempt "make distcheck" in this run?',
            name: 'DO_TEST_DISTCHECK')
        booleanParam (
            defaultValue: true,
            description: 'Attempt a "make install" check in this run?',
            name: 'DO_TEST_INSTALL')
        string (
            defaultValue: "`pwd`/tmp/_inst",
            description: 'If attempting a "make install" check in this run, what DESTDIR to specify? (absolute path, defaults to "BUILD_DIR/tmp/_inst")',
            name: 'USE_TEST_INSTALL_DESTDIR')
        booleanParam (
            defaultValue: false,
            description: 'Attempt "cppcheck" analysis before this run? (Note: corresponding tools are required in the build environment)',
            name: 'DO_CPPCHECK')
        booleanParam (
            defaultValue: true,
            description: 'Require that there are no files not discovered changed/untracked via .gitignore after builds and tests?',
            name: 'REQUIRE_GOOD_GITIGNORE')
        string (
            defaultValue: "30",
            description: 'When running tests, use this timeout (in minutes; be sure to leave enough for double-job of a distcheck too)',
            name: 'USE_TEST_TIMEOUT')
        booleanParam (
            defaultValue: true,
            description: 'When using temporary subdirs in build/test workspaces, wipe them after successful builds?',
            name: 'DO_CLEANUP_AFTER_BUILD')
    }
    triggers {
        pollSCM 'H/5 * * * *'
    }
// Note: your Jenkins setup may benefit from similar setup on side of agents:
//        PATH="/usr/lib64/ccache:/usr/lib/ccache:/usr/bin:/bin:${PATH}"
    stages {
        stage ('cppcheck') {
                    when { expression { return ( params.DO_CPPCHECK ) } }
                    steps {
                        dir("tmp") {
                            sh 'if [ -s Makefile ]; then make -k distclean || true ; fi'
                            sh 'chmod -R u+w .'
                            deleteDir()
                        }
                        sh 'cppcheck --std=c++11 --enable=all --inconclusive --xml --xml-version=2 . 2>cppcheck.xml'
                        archiveArtifacts artifacts: '**/cppcheck.xml'
                        sh 'rm -f cppcheck.xml'
                    }
        }
        stage ('prepare') {
                    steps {
                        dir("tmp") {
                            sh 'if [ -s Makefile ]; then make -k distclean || true ; fi'
                            sh 'chmod -R u+w .'
                            deleteDir()
                        }
                        sh './autogen.sh'
                        stash (name: 'prepped', includes: '**/*', excludes: '**/cppcheck.xml')
                    }
        }
        stage ('compile') {
            parallel {
                stage ('build with DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITH_DRAFT_API ) } }
                    steps {
                      dir("tmp/build-withDRAFT") {
                        deleteDir()
                        unstash 'prepped'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; ./configure --enable-drafts=yes --with-docs=no'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; make -k -j4 || make'
//                        sh 'if test -x src/.libs/lt-zsp ; then rm -f src/zsp ; fi'
                        sh 'rm -f src/zsp'
                        sh 'echo "Are GitIgnores good after make with drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        stash (name: 'built-draft', includes: '**/*', excludes: '**/cppcheck.xml')
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('build without DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITHOUT_DRAFT_API ) } }
                    steps {
                      dir("tmp/build-withoutDRAFT") {
                        deleteDir()
                        unstash 'prepped'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; ./configure --enable-drafts=no --with-docs=no'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; make -k -j4 || make'
//                        sh 'if test -x src/.libs/lt-zsp ; then rm -f src/zsp ; fi'
                        sh 'rm -f src/zsp'
                        sh 'echo "Are GitIgnores good after make without drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        stash (name: 'built-nondraft', includes: '**/*', excludes: '**/cppcheck.xml')
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('build with DOCS') {
                    when { expression { return ( params.DO_BUILD_DOCS ) } }
                    steps {
                      dir("tmp/build-DOCS") {
                        deleteDir()
                        unstash 'prepped'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; ./configure --enable-drafts=yes --with-docs=yes'
                        sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; make -k -j4 || make'
//                        sh 'if test -x src/.libs/lt-zsp ; then rm -f src/zsp ; fi'
                        sh 'rm -f src/zsp'
                        sh 'echo "Are GitIgnores good after make with docs? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        stash (name: 'built-docs', includes: '**/*', excludes: '**/cppcheck.xml')
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
            }
        }
        // Self-test stages below should be run sequentially, as decreed by
        // project authors for the time being (e.g. port conflicts, etc.)
        // You can uncomment the closures below experimentally, but proper
        // fix belongs in the project.xml (e.g. use separate agents if your
        // infrastructure is set up to only schedule one build on the agent
        // at a time) and better yet - in the project sources, to not have
        // the conflicts at all.
//        stage ('check') {
//            parallel {
                stage ('check with DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITH_DRAFT_API && params.DO_TEST_CHECK ) } }
                    steps {
//                      dir("tmp/test-check-withDRAFT") {
                      dir("tmp/build-withDRAFT") {
                        deleteDir()
                        unstash 'built-draft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" check'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make check with drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('check without DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITHOUT_DRAFT_API && params.DO_TEST_CHECK ) } }
                    steps {
//                      dir("tmp/test-check-withoutDRAFT") {
                      dir("tmp/build-withoutDRAFT") {
                        deleteDir()
                        unstash 'built-nondraft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" check'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make check without drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('memcheck with DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITH_DRAFT_API && params.DO_TEST_MEMCHECK ) } }
                    steps {
//                      dir("tmp/test-memcheck-withDRAFT") {
                      dir("tmp/build-withDRAFT") {
                        deleteDir()
                        unstash 'built-draft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" memcheck && exit 0 ; echo "Re-running failed ($?) memcheck with greater verbosity" >&2 ; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" VERBOSE=1 memcheck-verbose'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make memcheck with drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('memcheck without DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITHOUT_DRAFT_API && params.DO_TEST_MEMCHECK ) } }
                    steps {
//                      dir("tmp/test-memcheck-withoutDRAFT") {
                      dir("tmp/build-withoutDRAFT") {
                        deleteDir()
                        unstash 'built-nondraft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" memcheck && exit 0 ; echo "Re-running failed ($?) memcheck with greater verbosity" >&2 ; make LD_LIBRARY_PATH="$LD_LIBRARY_PATH" VERBOSE=1 memcheck-verbose'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make memcheck without drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('distcheck with DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITH_DRAFT_API && params.DO_TEST_DISTCHECK ) } }
                    steps {
//                      dir("tmp/test-distcheck-withDRAFT") {
                      dir("tmp/build-withDRAFT") {
                        deleteDir()
                        unstash 'built-draft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; DISTCHECK_CONFIGURE_FLAGS="--enable-drafts=yes --with-docs=no" ; export DISTCHECK_CONFIGURE_FLAGS; make DISTCHECK_CONFIGURE_FLAGS="$DISTCHECK_CONFIGURE_FLAGS" LD_LIBRARY_PATH="$LD_LIBRARY_PATH" distcheck'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make distcheck with drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('distcheck without DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITHOUT_DRAFT_API && params.DO_TEST_DISTCHECK ) } }
                    steps {
//                      dir("tmp/test-distcheck-withoutDRAFT") {
                      dir("tmp/build-withoutDRAFT") {
                        deleteDir()
                        unstash 'built-nondraft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh 'CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:$LD_LIBRARY_PATH"; export LD_LIBRARY_PATH; DISTCHECK_CONFIGURE_FLAGS="--enable-drafts=no --with-docs=no" ; export DISTCHECK_CONFIGURE_FLAGS; make DISTCHECK_CONFIGURE_FLAGS="$DISTCHECK_CONFIGURE_FLAGS" LD_LIBRARY_PATH="$LD_LIBRARY_PATH" distcheck'
                        }
                        }
                        sh 'echo "Are GitIgnores good after make distcheck without drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('install with DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITH_DRAFT_API && params.DO_TEST_INSTALL ) } }
                    steps {
//                      dir("tmp/test-install-withDRAFT") {
                      dir("tmp/build-withDRAFT") {
                        deleteDir()
                        unstash 'built-draft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh """CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:\${LD_LIBRARY_PATH}"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="\${LD_LIBRARY_PATH}" DESTDIR="${params.USE_TEST_INSTALL_DESTDIR}/withDRAFT" install"""
                        }
                        }
                        sh """cd "${params.USE_TEST_INSTALL_DESTDIR}/withDRAFT" && find . -ls"""
                        sh 'echo "Are GitIgnores good after make install with drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('install without DRAFT') {
                    when { expression { return ( params.DO_BUILD_WITHOUT_DRAFT_API && params.DO_TEST_INSTALL ) } }
                    steps {
//                      dir("tmp/test-install-withoutDRAFT") {
                      dir("tmp/build-withoutDRAFT") {
                        deleteDir()
                        unstash 'built-nondraft'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh """CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:\${LD_LIBRARY_PATH}"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="\${LD_LIBRARY_PATH}" DESTDIR="${params.USE_TEST_INSTALL_DESTDIR}/withoutDRAFT" install"""
                        }
                        }
                        sh """cd "${params.USE_TEST_INSTALL_DESTDIR}/withoutDRAFT" && find . -ls"""
                        sh 'echo "Are GitIgnores good after make install without drafts? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
                stage ('install with DOCS') {
                    when { expression { return ( params.DO_BUILD_DOCS && params.DO_TEST_INSTALL ) } }
                    steps {
//                      dir("tmp/test-install-withDOCS") {
                      dir("tmp/build-DOCS") {
                        deleteDir()
                        unstash 'built-docs'
                        retry(3) {
                        timeout (time: "${params.USE_TEST_TIMEOUT}".toInteger(), unit: 'MINUTES') {
                            sh """CCACHE_BASEDIR="`pwd`" ; export CCACHE_BASEDIR; LD_LIBRARY_PATH="`pwd`/src/.libs:\${LD_LIBRARY_PATH}"; export LD_LIBRARY_PATH; make LD_LIBRARY_PATH="\${LD_LIBRARY_PATH}" DESTDIR="${params.USE_TEST_INSTALL_DESTDIR}/withDOCS" install"""
                        }
                        }
                        sh """cd "${params.USE_TEST_INSTALL_DESTDIR}/withDOCS" && find . -ls"""
                        sh 'echo "Are GitIgnores good after make install with Docs? (should have no output below)"; git status -s || if [ "${params.REQUIRE_GOOD_GITIGNORE}" = false ]; then echo "WARNING GitIgnore tests found newly changed or untracked files" >&2 ; exit 0 ; else echo "FAILED GitIgnore tests" >&2 ; exit 1; fi'
                        script {
                            if ( params.DO_CLEANUP_AFTER_BUILD ) {
                                deleteDir()
                            }
                        }
                      }
                    }
                }
        // Sequential block of self-tests end here
//            }
//        }
        stage ('deploy if appropriate') {
            steps {
                script {
                    def myDEPLOY_JOB_NAME = sh(returnStdout: true, script: """echo "${params["DEPLOY_JOB_NAME"]}" """).trim();
                    def myDEPLOY_BRANCH_PATTERN = sh(returnStdout: true, script: """echo "${params["DEPLOY_BRANCH_PATTERN"]}" """).trim();
                    def myDEPLOY_REPORT_RESULT = sh(returnStdout: true, script: """echo "${params["DEPLOY_REPORT_RESULT"]}" """).trim().toBoolean();
                    echo "Original: DEPLOY_JOB_NAME : ${params["DEPLOY_JOB_NAME"]} DEPLOY_BRANCH_PATTERN : ${params["DEPLOY_BRANCH_PATTERN"]} DEPLOY_REPORT_RESULT : ${params["DEPLOY_REPORT_RESULT"]}"
                    echo "Used:     myDEPLOY_JOB_NAME:${myDEPLOY_JOB_NAME} myDEPLOY_BRANCH_PATTERN:${myDEPLOY_BRANCH_PATTERN} myDEPLOY_REPORT_RESULT:${myDEPLOY_REPORT_RESULT}"
                    if ( (myDEPLOY_JOB_NAME != "") && (myDEPLOY_BRANCH_PATTERN != "") ) {
                        if ( env.BRANCH_NAME =~ myDEPLOY_BRANCH_PATTERN ) {
                            def GIT_URL = sh(returnStdout: true, script: """git remote -v | egrep '^origin' | awk '{print \$2}' | head -1""").trim()
                            def GIT_COMMIT = sh(returnStdout: true, script: 'git rev-parse --verify HEAD').trim()
                            build job: "${myDEPLOY_JOB_NAME}", parameters: [
                                string(name: 'DEPLOY_GIT_URL', value: "${GIT_URL}"),
                                string(name: 'DEPLOY_GIT_BRANCH', value: env.BRANCH_NAME),
                                string(name: 'DEPLOY_GIT_COMMIT', value: "${GIT_COMMIT}")
                                ], quietPeriod: 0, wait: myDEPLOY_REPORT_RESULT, propagate: myDEPLOY_REPORT_RESULT
                        } else {
                            echo "Not deploying because branch '${env.BRANCH_NAME}' did not match filter '${myDEPLOY_BRANCH_PATTERN}'"
                        }
                    } else {
                        echo "Not deploying because deploy-job parameters are not set"
                    }
                }
            }
        }
    }
    post {
        success {
            script {
                if (currentBuild.getPreviousBuild()?.result != 'SUCCESS') {
                    // Uncomment desired notification

                    //slackSend (color: "#008800", message: "Build ${env.JOB_NAME} is back to normal.")
                    //emailext (to: "qa@example.com", subject: "Build ${env.JOB_NAME} is back to normal.", body: "Build ${env.JOB_NAME} is back to normal.")
                }
            }
        }
        failure {
            // Uncomment desired notification
            // Section must not be empty, you can delete the sleep once you set notification
            sleep 1
            //slackSend (color: "#AA0000", message: "Build ${env.BUILD_NUMBER} of ${env.JOB_NAME} ${currentBuild.result} (<${env.BUILD_URL}|Open>)")
            //emailext (to: "qa@example.com", subject: "Build ${env.JOB_NAME} failed!", body: "Build ${env.BUILD_NUMBER} of ${env.JOB_NAME} ${currentBuild.result}\nSee ${env.BUILD_URL}")
        }
    }
}
