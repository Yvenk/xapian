/* omenquireinternal.h: Internals
 *
 * ----START-LICENCE----
 * Copyright 1999,2000 BrightStation PLC
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 * -----END-LICENCE-----
 */

#ifndef OM_HGUARD_OMENQUIREINTERNAL_H
#define OM_HGUARD_OMENQUIREINTERNAL_H

#include "config.h"

#include <om/omenquire.h>
#include "omlocks.h"
#include "refcnt.h"
#include <algorithm>
#include <math.h>
#include <map>

class OmErrorHandler;

/** An item in the ESet.
 *  This item contains the termname, and the weight calculated for
 *  the document.
 */
class OmESetItem {
    public:
	OmESetItem(om_weight wt_, om_termname tname_)
		: wt(wt_), tname(tname_) {}
	/// Weight calculated.
	om_weight wt;
	/// Term suggested.
	om_termname tname;
	
	/** Returns a string representing the eset item.
	 *  Introspection method.
	 */
	std::string get_description() const;
};

class OmESetIterator::Internal {
    private:
	friend class OmESetIterator; // allow access to it
        friend bool operator==(const OmESetIterator &a, const OmESetIterator &b);

	std::vector<OmESetItem>::const_iterator it;
	std::vector<OmESetItem>::const_iterator end;
    
    public:
        Internal(std::vector<OmESetItem>::const_iterator it_,
		 std::vector<OmESetItem>::const_iterator end_)
	    : it(it_), end(end_)
	{ }

        Internal(const Internal &other) : it(other.it), end(other.end)
	{ }
};

/** An item resulting from a query.
 *  This item contains the document id, and the weight calculated for
 *  the document.
 */
class OmMSetItem {
    private:
    public:
	OmMSetItem(om_weight wt_, om_docid did_) : wt(wt_), did(did_) {}

	OmMSetItem(om_weight wt_, om_docid did_, const OmKey &key_)
		: wt(wt_), did(did_), collapse_key(key_) {}

	/** Weight calculated. */
	om_weight wt;

	/** Document id. */
	om_docid did;

	/** Key which was used to collapse upon.
	 *
	 *  If the collapse option is not being used, this will always
	 *  have a null value.
	 *
	 *  If a key of collapsing upon is specified, this will contain
	 *  the key for this particular item.  If the key is not present
	 *  for this item, the value will be a null string.  Only one
	 *  instance of each key value (apart from the null string) will
	 *  be present in the items in the returned OmMSet.
	 *
	 *  See the OmSettings match_collapse_key parameter for more
	 *  information about setting a key to collapse upon.
	 */
	OmKey collapse_key;

	/** Returns a string representing the mset item.
	 *  Introspection method.
	 */
	std::string get_description() const;
};

class OmMSetIterator::Internal {
    private:
	friend class OmMSetIterator; // allow access to it
        friend bool operator==(const OmMSetIterator &a, const OmMSetIterator &b);

	std::vector<OmMSetItem>::const_iterator it;
	std::vector<OmMSetItem>::const_iterator end;
    
    public:
        Internal(std::vector<OmMSetItem>::const_iterator it_,
		 std::vector<OmMSetItem>::const_iterator end_)
	    : it(it_), end(end_)
	{ }

        Internal(const Internal &other) : it(other.it), end(other.end)
	{ }
};

/** Internals of enquire system.
 *  This allows the implementation of OmEnquire to be hidden, allows
 *  cleaner pthread locking by separating the API calls from the internals,
 *  and allows the implementation to be shared with
 *  OmBatchEnquire::Internal.
 */
class OmEnquire::Internal {
    private:
	/// The database which this enquire object uses.
	const OmDatabase db;

	/** The user's query.
	 *  This may need to be mutable in future so that it can be
	 *  replaced by an optimised version.
	 */
	OmQuery * query;

	/// pthread mutex, used if available.
	OmLock mutex;

	/** Read a document from the database.
	 *  This method does the work for get_doc().
	 */
	const OmDocument read_doc(om_docid did) const;

	/** Calculate the matching terms.
	 *  This method does the work for get_matching_terms().
	 */
	om_termname_list calc_matching_terms(om_docid did) const;
    public:
	/** The error handler, if set.  (0 if not set).
	 */
	OmErrorHandler * errorhandler;

	Internal(const OmDatabase &databases, OmErrorHandler * errorhandler_);
	~Internal();

	void set_query(const OmQuery & query_);
	const OmQuery & get_query();
	OmMSet get_mset(om_doccount first,
			om_doccount maxitems,
			const OmRSet *omrset,
			const OmSettings *moptions,
			const OmMatchDecider *mdecider) const;
	OmESet get_eset(om_termcount maxitems,
			const OmRSet & omrset,
			const OmSettings *eoptions,
			const OmExpandDecider *edecider) const;
	const OmDocument get_doc(om_docid did) const;
	const OmDocument get_doc(const OmMSetIterator &it) const;

	const std::vector<OmDocument> get_docs(
		std::vector<OmMSetIterator>::const_iterator begin,
		std::vector<OmMSetIterator>::const_iterator end) const;

	om_termname_list get_matching_terms(om_docid did) const;
	om_termname_list get_matching_terms(const OmMSetIterator &it) const;
	std::string get_description() const;
};

class OmExpand;

class OmMSet::Internal {
    friend class OmMSet;
    friend class MSetPostList;
    friend std::string ommset_to_string(const OmMSet &ommset);
    private:
	/** The term frequencies and weights returned by the match process.
	 *  This map will contain information for each term which was in                 *  the query.
	 */
	std::map<om_termname, OmMSet::TermFreqAndWeight> termfreqandwts;

	/// A list of items comprising the (selected part of the) mset.
	std::vector<OmMSetItem> items;

	om_doccount firstitem;

	om_doccount matches_lower_bound;

	om_doccount matches_estimated;

	om_doccount matches_upper_bound;

	om_weight max_possible;

	om_weight max_attained;

    public:
	Internal()
		: firstitem(0),
		  matches_lower_bound(0),
		  matches_estimated(0),
		  matches_upper_bound(0),
		  max_possible(0),
		  max_attained(0) {}

	Internal(om_doccount firstitem_,
		 om_doccount matches_upper_bound_,
		 om_doccount matches_lower_bound_,
		 om_doccount matches_estimated_,
		 om_weight max_possible_,
		 om_weight max_attained_,
		 const std::vector<OmMSetItem> &items_,
		 const std::map<om_termname, TermFreqAndWeight> &termfreqandwts_)
		: termfreqandwts(termfreqandwts_),
		  items(items_),
		  firstitem(firstitem_),
		  matches_lower_bound(matches_lower_bound_),
		  matches_estimated(matches_estimated_),
		  matches_upper_bound(matches_upper_bound_),
		  max_possible(max_possible_),
		  max_attained(max_attained_) {}

	/** Returns a string representing the mset.
	 *  Introspection method.
	 */
	std::string get_description() const;
};

class OmESet::Internal {
    friend class OmESet;
    friend class OmExpand;
    private:
	/// A list of items comprising the (selected part of the) eset.
	std::vector<OmESetItem> items;

	/** A lower bound on the number of terms which are in the full
	 *  set of results of the expand.  This will be greater than or
	 *  equal to items.size()
	 */
	om_termcount ebound;

    public:
	Internal() : ebound(0) {}

	/** Returns a string representing the eset.
	 *  Introspection method.
	 */
	std::string get_description() const;
};

class RSet;

class OmRSet::Internal {
    friend class OmRSet;
    friend class RSet;
    friend class OmExpand;
    friend class MultiMatch;
    friend std::string omrset_to_string(const OmRSet &omrset);

    private:
	/// Items in the relevance set.
	std::set<om_docid> items;

    public:
	/** Returns a string representing the rset.
	 *  Introspection method.
	 */
	std::string get_description() const;
};

#endif // OM_HGUARD_OMENQUIREINTERNAL_H
