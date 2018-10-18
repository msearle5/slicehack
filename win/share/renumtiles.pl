#!\usr\bin\perl
use strict;
use warnings;

my $fp = $ARGV[0] or die("First argument should be the input file");
my $op = "$ARGV[0] modified";
my $num = 0;
open(INPUT, "<$fp") or die("Could not open $fp");
open(OUTPUT, ">$op") or die("Could not open output file");

while(<INPUT>) {
	if ($_ =~ s/tile [0-9]+/tile \Q$num\E/i) {
		$num++;
	}
	print OUTPUT "$_";
}
close(INPUT);
close(OUTPUT);
