#!/usr/bin/perl -w
#
# Copyright (C) 2009 Adam Dingle <adam@yorba.org>
#
# This file is part of WebKit
# 
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
# 
# You should have received a copy of the GNU Library General Public License
# aint with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.
# 

my $classlist = <STDIN>;
chomp($classlist);
my @classes = split / /, $classlist;
@classes = sort @classes;

print <<EOF;
/* This file is part of the WebKit open source project.
   This file has been generated by gobject-generate-headers.pl.  DO NOT MODIFY!
   
   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

EOF

my $outType = $ARGV[0];
my $header;
if ($outType eq "defines") {
    $header = "webkitdomdefines_h";
} elsif ($outType eq "gdom") {
    $header = "webkitdom_h";
} else {
    die "unknown output type";
}

print "#ifndef ${header}\n";
print "#define ${header}\n";
print "\n";

if ($outType eq "defines") {
    print "#include <glib.h>\n\n";
    print "#ifdef G_OS_WIN32\n";
    print "    #ifdef BUILDING_WEBKIT\n";
    print "        #define WEBKIT_API __declspec(dllexport)\n";
    print "    #else\n";
    print "        #define WEBKIT_API __declspec(dllimport)\n";
    print "    #endif\n";
    print "#else\n";
    print "    #define WEBKIT_API __attribute__((visibility(\"default\")))\n";
    print "#endif\n\n";
    print "#define WEBKIT_DEPRECATED WEBKIT_API G_DEPRECATED\n";
    print "#define WEBKIT_DEPRECATED_FOR(f) WEBKIT_API G_DEPRECATED_FOR(f)\n";
    print "\n";
    print "#ifndef WEBKIT_API\n";
    print "    #define WEBKIT_API\n";
    print "#endif\n";

    foreach my $class (@classes) {
        print "typedef struct _WebKitDOM${class} WebKitDOM${class};\n";
        print "typedef struct _WebKitDOM${class}Class WebKitDOM${class}Class;\n";
        print "\n";
    }
} elsif ($outType eq "gdom") {
    print "#define __WEBKITDOM_H_INSIDE__\n\n";
    foreach my $class (@classes) {
        print "#include <webkitdom/WebKitDOM${class}.h>\n";
    }
    print "\n#undef __WEBKITDOM_H_INSIDE__\n";
}

print "\n";
print "#endif\n";
