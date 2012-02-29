<?php

require 'MustacheNative.php';

// Meh
error_reporting(E_ALL | E_STRICT);
ini_set('memory_limit', '256M');
ini_set('display_errors', true);

// Argv
if( !empty($argv[1]) && is_dir($argv[1]) ) {
  $specDir = $argv[1];
} else if( is_dir('./specs') ) {
  $specDir = './specs';
} else if( is_dir('./spec/specs') ) {
  $specDir = './spec/specs';
} else {
  echo 'Unable to find specs' . PHP_EOL;
  exit(1);
}

$specs = array();
foreach( scandir($specDir) as $file ) {
  if( $file[0] == '~' ) {
    continue;
  }
  if( strlen($file) > 5 && substr($file, -5) == '.json' ) {
    $specs[] = substr($file, 0, -5);
  }
}

$specData = array();
foreach( $specs as $spec ) {
  $specData[$spec] = json_decode(file_get_contents($specDir . DIRECTORY_SEPARATOR . $spec . '.json'), true);
}



// Run tests
$mustache = new MustacheNative();
$results = array();
$nFailed = 0;
$nPassed = 0;

foreach( $specData as $spec => $data ) {
  $tests = $data['tests'];
  foreach( $tests as $test ) {
    try {
      $given = $mustache->render($test['template'], $test['data']);
      $error = null;
    } catch( Exception $e ) {
      $given = null;
      $error = $e;
    }
    if( $given !== $test['expected'] ) {
      $results[$spec][] = array(
        'test' => $test,
        'given' => $given,
        'expected' => $test['expected'],
        'result' => false,
        'error' => $error,
      );
      $nFailed++;
      printf("[%s] %s %s" . PHP_EOL, $spec, $test['name'], 'FAILED');
      print("    Template: " . addcslashes($test['template'], "\n\r\t") . PHP_EOL);
      print("    Expected: " . addcslashes($test['expected'], "\n\r\t") . PHP_EOL);
      print("    Given: " . addcslashes($given, "\n\r\t") . PHP_EOL);
      if( $error ) {
        print("    Error: " . $error->getMessage() . PHP_EOL);
      }
    } else {
      $results[$spec][] = array(
        'test' => $test,
        'given' => $given,
        'expected' => $test['expected'],
        'result' => true,
        'error' => $error,
      );
      $nPassed++;
      printf("[%s] %s %s" . PHP_EOL, $spec, $test['name'], 'FAILED');
    }
  }
}

printf('Pass/Fail: %d/%d' . PHP_EOL, $nPassed, $nFailed);