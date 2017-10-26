#!groovy

def specToLabel(Map spec) {
  return "${spec.os}-${spec.arch}-${spec.compiler}-FairSoft_${spec.fairsoft}"
}

def buildMatrix(List specs, Closure callback) {
  def nodes = [:]
  for (spec in specs) {
    def label = specToLabel(spec)
    def cdashUrl = 'https://cdash.gsi.de/index.php?project=FairRoot#!#Experimental'
    nodes[label] = {
      node(label) {
        githubNotify(context: "alfa-ci/${label}", description: 'Building ...', status: 'PENDING')
        try {
          deleteDir()
          checkout scm

          callback.call(spec, label)

          deleteDir()
          githubNotify(context: "alfa-ci/${label}", description: 'Success', status: 'SUCCESS', targetUrl: cdashUrl)
        } catch (e) {
          deleteDir()
          githubNotify(context: "alfa-ci/${label}", description: 'Error', status: 'ERROR', targetUrl: cdashUrl)
          throw e
        }
      }
    }
  }
  return nodes
}

pipeline{
  agent none
  stages {
    stage("Run Build/Test Matrix") {
      steps{
        script {
          parallel(buildMatrix([
            [os: 'Debian8',    arch: 'x86_64', compiler: 'gcc4.9',         fairsoft: 'oct17'],
            [os: 'MacOS10.11', arch: 'x86_64', compiler: 'AppleLLVM8.0.0', fairsoft: 'oct17'],
            [os: 'MacOS10.13', arch: 'x86_64', compiler: 'AppleLLVM9.0.0', fairsoft: 'oct17'],
          ]) { spec, label ->
            sh '''\
              echo "export BUILDDIR=$PWD/build" >> Dart.cfg
              echo "export SOURCEDIR=$PWD" >> Dart.cfg
              echo "export PATH=$SIMPATH/bin:$PATH" >> Dart.cfg
              echo "export GIT_BRANCH=$JOB_BASE_NAME" >> Dart.cfg
            '''
            sh 'cmake --version'
            sh 'env'
            sh './Dart.sh jenkins Dart.cfg'
          })
        }
      }
    }
  }
}
