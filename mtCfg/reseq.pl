#!/usr/bin/perl -w

use strict;

#-------------------------------------------------------------------------------

sub sizeOf($)
{
   my ($line) = @_;

   my $size = 0;

   if (m/= array/) {
      m/[US](\d\d).+\[(.*)\]/;
      my $bytes = $1/8;
      my $count = $2;
      $count =~ s/x/ * /g;
      $count = eval($count);
      $size  = $count * $bytes;
   }
   elsif (m/= scalar/ || m/= bits/) {
      m/[US](\d\d)/;
      $size = $1/8;
   }

   return $size;
}

#-------------------------------------------------------------------------------

sub fixLine($$)
{
   my ($line, $value) = @_;

   my $sValue = sprintf("%4d", $value);
   $line =~ s/(, +)####,/$1$sValue,/;
   return $line;
}
   
#-------------------------------------------------------------------------------


my $resequencing = 0;
my $size         = 0;
my $offset       = 0;
my $aliasSize    = 0;
my $aliasOffset  = 0;
my $options      = "";
my $optionOffset = 0;

# Options:
#   B = begin processing block
#   E = end processing block
#   A = this element is an alias
#   R = this element is a reset alias
#   n = number indicating offset of this alias block, only on A

while (<>) {
   s/[\r\n]//g;

   if (s/@\(([^)]*)\)//) { $options = $1; }
   else                  { $options = ""; }
   if ($options =~ m/(\d+)/) {
      $optionOffset = $1;
   }
   else {
      $optionOffset = 0;
   }

   if ($options =~ m/B/) {
      $resequencing = 1;
      $size         = 0;
      $offset       = 0;
      $aliasSize    = 0;
      $aliasOffset  = 0;
   }

   if ($options =~ m/E/) {
      $resequencing = 0;
      printf ";     pageSize = %4d\r\n", $offset;
   }

   if ($resequencing) {
      if ($options =~ m/A/) {
         if (m/, +####,/) {
            $aliasSize   = sizeOf($_);
            $aliasOffset = ($offset-$size) if $options =~ m/R/;
            $aliasOffset -= $optionOffset;
            $_ = fixLine($_, $aliasOffset);
            $aliasOffset += $aliasSize;
         }
      }
      else {
         if (m/, +####,/) {
            $size = sizeOf($_);
            $aliasOffset = $offset; # Implicit reset
            $_ = fixLine($_, $offset);
            $offset += $size;
         }
      }
   }

   s/ +$//;
   print "$_\r\n";
}

#-------------------------------------------------------------------------------
