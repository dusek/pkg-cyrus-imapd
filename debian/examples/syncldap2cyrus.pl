#!/usr/bin/perl -w
use strict;
use vars qw($opt_a $opt_f $opt_n $opt_r);
use Getopt::Std;
use Net::LDAP;
use Cyrus::IMAP::Admin;

# Requires: Getopt::Std, Net::LDAP, Cyrus::IMAP::Admin
# Debian pkgs: libnet-ldap-perl, libcyrus-imap-perl21

# Script Title : syncldap2cyrus.pl
# Written By   : Stuart Young (aka Cef) <cef@optus.net>
# Last Modified: Mon Mar 14 23:59:14 UTC 2005
# Description:
#  syncldap2cyrus.pl retrieves a list of usernames from an LDAP server
#  and then tries to create a user mailbox in Cyrus, and then any
#  secondary mailboxes under that. The idea is that you could run this
#  via cron or manually every time you add a user to your LDAP server,
#  and they would then have a Cyrus mailbox. This script also has a
#  report mode that shows you any users in LDAP that do not have a
#  Cyrus mailbox, and vice versa.

# Notes:
#  This script is FREE. There is no support. If it breaks, you get to
#  keep all the pieces.
#  Please do not complain about the quality or style of this script,
#  or you will be ignored.
#  If you want to add SSL/TLS and/or inbuilt SASL support, go for it!
#  Yes I know it will use a lot of memory if there are lots of users
#  and/or mailboxes. Live with it, or rewrite it - your call.

sub Usage ()
{
  return <<END;
usage: syncldap2cyrus.pl [-a|-f|-n|-r]

 -a : Add all users from LDAP to Cyrus
 -f : Force creation of mailboxes even if they have errors (implies -a)
 -n : Do not add users to Cyrus, just display (implies -a)
 -r : Report on differences between LDAP and Cyrus only (implies -n)
END
}

# No options, give a Usage message
(scalar(@ARGV)) or
  die Usage();

getopts('afnr');

# Get options
my $addcyrus = 1;
my $forcecreate = 0;
my $report = 0;
my $go = 0;

if (defined($opt_a))
{
  $go = 1;
}
if (defined($opt_f))
{
  $go = 1;
  $forcecreate = 1;
}
if (defined($opt_n))
{
  $go = 1;
  $addcyrus = 0;
}
if (defined($opt_r))
{
  $go = 1;
  $report = 1;
  $addcyrus = 0;
}

# No regognised options, give a usage message
if ($go == 0)
{
  die Usage();
}

# Settings - should come from config file(s) at some point

# Debug mode, normally 0
my $debug = 0;
# Cyrus Host to connect to
my $cyrus_host = 'localhost';
# Cyrus admin user & password
my $cyrus_admin = 'cyrus';
my $cyrus_pass = 'secret';
# Cyrus Unix Seperator - allow .'s in names (unixhierarchysep in cyrus)
my $cyrus_unix_sep = 0;
# LDAP server uri
my $ldap_uri = 'ldap://localhost/';
# LDAP bind domain & password
my $ldap_bind_domain = 'cn=Manager, dc=example, dc=com';
my $ldap_bind_pass = 'secret';
# LDAP search base
my $ldap_search_base = 'ou=People, dc=example, dc=com';
# LDAP search scope
my $ldap_search_scope = 'sub';
# LDAP user attribute to return.
my $ldap_user_attr = 'uid';
# LDAP search query. Where possible, use $ldap_attr here to avoid
# problems when changing the query
my $ldap_search_query = "($ldap_user_attr=*)";
# List of mailboxes to create for each user
my @mboxlist = ('Trash', 'Sent', 'Spam', 'Drafts');

# General vars
my $count = 0;
my $realuser = '';
my $done = 0;

# Connect to LDAP and bind
my $ldap = Net::LDAP->new($ldap_uri) or
  die "Can't connect to LDAP server at " . $ldap_uri;
my $mesg = $ldap->bind ($ldap_bind_domain, password => $ldap_bind_pass);

$mesg = $ldap->search ( base => $ldap_search_base,
                        scope => $ldap_search_scope,
                        filter => $ldap_search_query,
                        attr => $ldap_user_attr );

$mesg->code && die "LDAP error: " . $mesg->error;

# Connect to Cyrus via IMAP
my $client = 0;
my $rc = 0;
my $sepchar = '.';

$client = Cyrus::IMAP::Admin->new("$cyrus_host");
if ($debug == 1)
{
  print "Connecting to Cyrus using user: ". $cyrus_admin. "\n";
}
$rc = $client->authenticate(-user => $cyrus_admin, -password => $cyrus_pass) or
  die "Can't authenticate to Cyrus on $cyrus_host as user: ". $cyrus_admin. "\n";
if ($cyrus_unix_sep == 1)
{
  $sepchar = '/';
}
my $prefix = "user".$sepchar;

# Populate the LDAP user list - Very messy but works
my $ldaprawdata = '';
my $ldapfieldattr = '';
my $ldapfielddata = '';
my $ldaphref = $mesg->as_struct;
my @ldapuserlist = ();
foreach (keys %$ldaphref)
{
  $ldaprawdata = $$ldaphref{$_};
  my @ldapsortattr = sort keys %$ldaprawdata;
  foreach $ldapfieldattr (@ldapsortattr)
  {
    next if ( $ldapfieldattr =~ /;binary$/ );
    my $ldapfielddata = @$ldaprawdata{$ldapfieldattr};
    if ($debug == 1)
    {
      print "$ldapfieldattr = @$ldapfielddata\n";
    }
    if ( $ldapfieldattr eq $ldap_user_attr )
    {
      $ldapuserlist[$count] = "@$ldapfielddata";
    }
  }
  $count += 1;
}

my $maildata = '';
my @mailboxes = $client->list('%', $prefix);
my @cyrusmboxes = ();
if ($debug == 1)
{
  print "\nThere are ".@ldapuserlist," LDAP users\n";
  print "\nThere are ".@mailboxes," Cyrus mailboxes\n";
}

$count = 0;

# Populate the Cyrus mailbox list
foreach $maildata (@mailboxes)
{
  # Note: Element 0 = full mailbox name/path
  if ($addcyrus == 0 && $report == 0)
  {
    print "Cyrus: @{$maildata}[0]\n";
  }
  # Remove the Cyrus user part
  $cyrusmboxes[$count] = substr(@{$maildata}[0],length($prefix),length(@{$maildata}[0]));
  $count += 1;
}

my $item = '';
my %seen = ();
my @answer = ();

if ($report == 1)
{
  $item = '';
  %seen = ();
  @answer = ();
  $count = 0;
  # Which users are in Cyrus but not in LDAP?
  @seen{@ldapuserlist} = ();
  foreach $item (@cyrusmboxes)
  {
    push (@answer, $item) unless exists $seen{$item};
  }
  foreach $item (@answer)
  {
    print "In Cyrus but not LDAP: ". $item ."\n";
    $count += 1;
  }

  $item = '';
  %seen = ();
  @answer = ();
  # Which mailboxes are in LDAP, but not in Cyrus?
  @seen{@cyrusmboxes} = ();
  foreach $item (@ldapuserlist)
  {
    push (@answer, $item) unless exists $seen{$item};
  }
  foreach $item (@answer)
  {
    print "In LDAP but not Cyrus: ". $item ."\n";
    $count += 1;
  }
  if ($count == 0 && $debug == 1)
  {
    print "No differences.\n";
  }
}
else
{
  # Iterate over the LDAP user list
  $count = 0;
  foreach $realuser (@ldapuserlist) 
  {
    if ($addcyrus == 0)
    {
      # Display matching ldap record
      print "LDAP : $realuser\n";
    }
    elsif ($addcyrus == 1)
    {
      $done = 0;
      # We check if the Cyrus mailbox exists first, and if not, create it
      # We ALWAYS check each individual user mailbox too, in case the list
      # in the var @mailboxes gets updated
      if ($client->list("$prefix$realuser") && $forcecreate == 0)
      {
        if ($debug == 1)
        {
          print "Mailbox already exists for $realuser\n";
        }
      }
      elsif ($client->create("$prefix$realuser"))
      {
        $done += 1;
        if ($client->setacl("$prefix$realuser", ${cyrus_admin} => "cd"))
        {
          if ($debug == 1)
          {
            print "Admin permissions set on $prefix$realuser\n"
          }
        }
        else
        {
          print STDERR "Couldn't set $cyrus_admin\'s ACL on mailbox $prefix$realuser\: " . $client->error . "\n";
          if ($forcecreate == 0)
          {
            exit 1;
          }
        }
      }
      else
      {
        print STDERR "Couldn't create mailbox $prefix$realuser\: " . $client->error . "\n";
        if ($forcecreate == 0)
        {
          exit 1;
        }
      }
      foreach my $mailbox ( @mboxlist )
      {
        if ($client->list("$prefix$realuser$sepchar$mailbox"))
        {
          if ($debug)
          {
            print "Mailbox $mailbox already exists for $realuser\n";
          }
        }
        elsif ($client->create("$prefix$realuser$sepchar" . ${mailbox}))
        {
          $done += 1;
        }
        else
        {
          print STDERR  "Couldn't create mailbox $prefix$realuser$sepchar" . ${mailbox}.": " . $client->error . "\n";
          if ($forcecreate == 0)
          {
            exit 1;
          }
        }
      }
      if ($done > 0)
      {
        print "User $realuser\'s mailboxes created successfully\n";
        $count += 1;
      }
    }
  }
  if ($count > 0)
  {
    print "Created $count user mailboxes\n";
  }
}

$mesg = $ldap->unbind;
exit 0;
