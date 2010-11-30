This perl wrapper lets you replace it with the usual command line perl executable (provided you point it to the valid runtime) and
then it will attempt to catch exploit attempts, and log what it's doing.

##To install:

Line 86 in wrap.php needs to be customized (customize the name of the real perl executable).

Our example is /usr/bin/perl_real.

So for install, you would do the following:

    cp -a /usr/bin/perl /usr/bin/perl_real
    ./build.sh
    cp perl /usr/bin/perl
