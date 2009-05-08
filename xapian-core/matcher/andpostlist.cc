/* andpostlist.cc: Return only items which are in both sublists
 *
 * Copyright 1999,2000,2001 BrightStation PLC
 * Copyright 2002 Ananova Ltd
 * Copyright 2003,2004,2007,2008,2009 Olly Betts
 * Copyright 2007,2009 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include <config.h>

#include "andpostlist.h"
#include "omassert.h"
#include "omdebug.h"

inline void
AndPostList::process_next_or_skip_to(Xapian::weight w_min, PostList *ret)
{
    DEBUGCALL(MATCH, void, "AndPostList::process_next_or_skip_to",
	      w_min << ", " << ret);
    head = 0;
    handle_prune(r, ret);
    LOGVALUE(MATCH, r->at_end());
    if (r->at_end()) return;

    // r has just been advanced by next or skip_to so must be > head
    // (and head is the current position of l)
    Xapian::docid rhead = r->get_docid();
    LOGVALUE(MATCH, rhead);
    LOGVALUE(MATCH, w_min);
    LOGVALUE(MATCH, rmax);
    skip_to_handling_prune(l, rhead, w_min - rmax, matcher);
    LOGVALUE(MATCH, l->at_end());
    if (l->at_end()) return;

    Xapian::docid lhead = l->get_docid();
    LOGVALUE(MATCH, lhead);

    while (lhead != rhead) {
	if (lhead < rhead) {
	    // FIXME: CSE these w_min values?
	    // But note that lmax and rmax may change on recalc_maxweight...
	    skip_to_handling_prune(l, rhead, w_min - rmax, matcher);
	    LOGVALUE(MATCH, l->at_end());
	    if (l->at_end()) {
		head = 0;
		return;
	    }
	    lhead = l->get_docid();
	    LOGVALUE(MATCH, lhead);
	} else {
	    skip_to_handling_prune(r, lhead, w_min - lmax, matcher);
	    LOGVALUE(MATCH, r->at_end());
	    if (r->at_end()) {
		head = 0;
		return;
	    }
	    rhead = r->get_docid();
	    LOGVALUE(MATCH, rhead);
	}
    }

    head = lhead;
    return;
}

AndPostList::AndPostList(PostList *left_, PostList *right_,
			 MultiMatch *matcher_,
			 Xapian::doccount dbsize_,
			 bool replacement)
	: BranchPostList(left_, right_, matcher_), head(0), lmax(0), rmax(0),
	  dbsize(dbsize_)
{
    DEBUGCALL(MATCH, void, "AndPostList", left_ << ", " << right_ << ", " << matcher_ << ", " << dbsize_ << ", " << replacement);
    if (l->get_termfreq_est() > r->get_termfreq_est()) swap(l, r);
    if (replacement) {
	// Initialise the maxweights from the kids so we can avoid forcing
	// a full maxweight recalc
	lmax = l->get_maxweight();
	rmax = r->get_maxweight();
    }
}

PostList *
AndPostList::next(Xapian::weight w_min)
{
    DEBUGCALL(MATCH, PostList *, "AndPostList::next", w_min);
    process_next_or_skip_to(w_min, r->next(w_min - lmax));
    RETURN(NULL);
}

PostList *
AndPostList::skip_to(Xapian::docid did, Xapian::weight w_min)
{
    DEBUGCALL(MATCH, PostList *, "AndPostList::skip_to", did << ", " << w_min);
    if (did > head)
	process_next_or_skip_to(w_min, r->skip_to(did, w_min - lmax));
    RETURN(NULL);
}

Xapian::doccount
AndPostList::get_termfreq_max() const
{
    DEBUGCALL(MATCH, Xapian::doccount, "AndPostList::get_termfreq_max", "");
    RETURN(std::min(l->get_termfreq_max(), r->get_termfreq_max()));
}

Xapian::doccount
AndPostList::get_termfreq_min() const
{
    DEBUGCALL(MATCH, Xapian::doccount, "AndPostList::get_termfreq_min", "");
    // To have minimum matching documents, sets of documents matching both
    // components of the AND must be minimal in size, and maximally disjoint.
    //
    // The overlap is then given by:
    //  = lower_bound(left) + lower_bound(right) - dbsize
    Xapian::doccount lmin = l->get_termfreq_min();
    Xapian::doccount sum = lmin + r->get_termfreq_min();
    // If sum < lmin, then the calculation overflowed and the true value of
    // sum must be > dbsize.
    if (sum < lmin || sum > dbsize) {
	RETURN(sum - dbsize);
    }
    RETURN(0u);
}

Xapian::doccount
AndPostList::get_termfreq_est() const
{
    DEBUGCALL(MATCH, Xapian::doccount, "AndPostList::get_termfreq_est", "");
    // Estimate assuming independence:
    // P(l and r) = P(l) . P(r)
    double lest = static_cast<double>(l->get_termfreq_est());
    double rest = static_cast<double>(r->get_termfreq_est());
    RETURN(static_cast<Xapian::doccount>(lest * rest / dbsize + 0.5));
}

TermFreqs
AndPostList::get_termfreq_est_using_stats(
	const Xapian::Weight::Internal & stats) const 
{
    LOGCALL(MATCH, TermFreqs,
	    "AndPostList::get_termfreq_est_using_stats", stats);
    // Estimate assuming independence:
    // P(l and r) = P(l) . P(r)
    TermFreqs lfreqs(l->get_termfreq_est_using_stats(stats));
    TermFreqs rfreqs(r->get_termfreq_est_using_stats(stats));

    double freqest, relfreqest;

    // Our caller should have ensured this.
    Assert(stats.collection_size);

    freqest = double(lfreqs.termfreq) *
	    double(rfreqs.termfreq) / stats.collection_size;

    if (stats.rset_size == 0) {
	relfreqest = 0;
    } else {
	relfreqest = double(lfreqs.reltermfreq) *
		double(rfreqs.reltermfreq) / stats.rset_size;
    }

    RETURN(TermFreqs(static_cast<Xapian::doccount>(freqest + 0.5),
		     static_cast<Xapian::doccount>(relfreqest + 0.5)));
}

Xapian::docid
AndPostList::get_docid() const
{
    DEBUGCALL(MATCH, Xapian::docid, "AndPostList::get_docid", "");
    RETURN(head);
}

// only called if we are doing a probabilistic AND
Xapian::weight
AndPostList::get_weight() const
{
    DEBUGCALL(MATCH, Xapian::weight, "AndPostList::get_weight", "");
    RETURN(l->get_weight() + r->get_weight());
}

// only called if we are doing a probabilistic operation
Xapian::weight
AndPostList::get_maxweight() const
{
    DEBUGCALL(MATCH, Xapian::weight, "AndPostList::get_maxweight", "");
    RETURN(lmax + rmax);
}

Xapian::weight
AndPostList::recalc_maxweight()
{
    DEBUGCALL(MATCH, Xapian::weight, "AndPostList::recalc_maxweight", "");
    lmax = l->recalc_maxweight();
    rmax = r->recalc_maxweight();
    RETURN(AndPostList::get_maxweight());
}

bool
AndPostList::at_end() const
{
    DEBUGCALL(MATCH, bool, "AndPostList::at_end", "");
    RETURN(head == 0);
}

std::string
AndPostList::get_description() const
{
    return "(" + l->get_description() + " And " + r->get_description() + ")";
}

Xapian::termcount
AndPostList::get_doclength() const
{
    DEBUGCALL(MATCH, Xapian::termcount, "AndPostList::get_doclength", "");
    LOGLINE(MATCH, "docid=" << head);
    Xapian::termcount doclength = l->get_doclength();
    AssertEq(doclength, r->get_doclength());
    RETURN(doclength);
}

Xapian::termcount
AndPostList::get_wdf() const
{
    DEBUGCALL(MATCH, Xapian::termcount, "AndPostList::get_wdf", "");
    RETURN(l->get_wdf() + r->get_wdf());
}