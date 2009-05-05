# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

#########################

# change 'tests => 1' to 'tests => last_test_to_print';

use Test;
use Devel::Peek;
BEGIN { plan tests => 14 };
use Search::Xapian qw(:ops);

#########################

# Insert your test code below, the Test module is use()ed here so read
# its man page ( perldoc Test ) for help writing this test script.

# None of the following tests can be expected to succeed without first
# creating a test database in the directory testdb.

my $db = Search::Xapian::Database->new( 'testdb' );
my $enq = Search::Xapian::Enquire->new( $db );
my $query = Search::Xapian::Query->new( 'test' );

$enq->set_query( $query );

my $mset;
ok( $mset = $enq->get_mset(0, 10) );
my @matches;
ok( @matches = $mset->matches() );
my $match;
ok( $match = $matches[0] );
ok( $match->get_docid() );
ok( $match->get_percent() );

$matches[0] = 34;

my $doc;
ok( $doc = $match->get_document() );
ok( $doc->get_data() );

ok( exists $matches[1] );
ok( !exists $matches[10] );
ok( exists $matches[-1] );

my @ematches;
ok( @ematches = $enq->matches(0, 2) );

ok( $match = $ematches[0] );
ok( $match->get_docid() );
ok( $match->get_percent() );

1;