node('master') {
    stage('Build') {
        sh "echo Build"
    }
    stage('Test'){
      parallel (
        "JUnit": { 
            sh "echo JUnit"
        },
        "DBUnit": { 
            sh "echo DBUnit"
        },
        "Jasmine": { 
            sh "echo Jasmine"
        },
      )
    }
    stage('Browser Tests'){
      parallel (
        "Firefox": { 
            sh "echo Firefox"
        },
        "Edge": { 
            sh "echo Edge"
        },
        "Safari": { 
            sh "echo Safari"
        },
        "Chrome": { 
            sh "echo Chrome"
        },
      )
    }
    stage('Dev'){
        sh "echo Dev"
    }
    stage('Staging'){
        sh "echo Staging"
    }
    stage('Production'){
        sh "echo Production"
    }
}
