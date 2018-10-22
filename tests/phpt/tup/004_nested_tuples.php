@ok
<?php
require_once 'polyfill/tuple-php-polyfill.php';
require_once 'Classes/autoload.php';

function getT1($arg) {
    return tuple($arg, [$arg]);
}

function getT2() {
    return tuple(getT1(1), 'str');
}

function demo() {
    $a = tuple(1,'string', tuple(1, 'str2', [1,2,3,'vars']), new Classes\A);

    $int = $a[0];
    $string = $a[1];
    $in_tuple = $a[2];
    /** @var Classes\A */
    $a_inst = $a[3];
    $int_of_inner_tuple = $a[2][0];
    $str_of_inner_tuple = $in_tuple[1];
    $var_2_of_inner_tuple_array = $a[2][2][2];

    // check that types are correctly inferred
    $int /*:= int */;
    $string /*:= string */;
    $int_of_inner_tuple /*:= int */;
    $str_of_inner_tuple /*:= string */;

    echo $int, "\n";
    echo $string, "\n";
    echo $a_inst->setA(9)->a, "\n";
    echo $int_of_inner_tuple, "\n";
    echo $str_of_inner_tuple, "\n";
    echo $var_2_of_inner_tuple_array, "\n";
}

function demo2() {
    $t2 = getT2();                  // tuple <tuple<...>, string>
    $str = $t2[1];
    $int = $t2[0][1][0];            // [0] is tuple, [0][1] is int[]
    echo $int, " ", $str, "\n";

    // check that types are correctly inferred
    $int /*:= int */;
    $str /*:= string */;
}

demo();
demo2();