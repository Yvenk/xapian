/** @file featuremanager.cc
 * @brief FeatureManager class
 */
/* Copyright (C) 2011 Parth Gupta
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
#include <xapian.h>
#include <config.h>

#include "xapian-letor/featuremanager.h"
#include "xapian-letor/featurevector.h"
#include "xapian-letor/ranklist.h"
#include "featuremanager_internal.h"

#include <cstring>
#include <cstdlib>
#include <fstream>

#include <map>
#include <string>

using namespace std;

namespace Xapian {

FeatureManager::FeatureManager() : internal(new FeatureManager::Internal)
{
}

FeatureManager &
FeatureManager::operator=(const FeatureManager & o)
{
    internal = o.internal;
    return *this;
}

FeatureManager::FeatureManager(const FeatureManager & o) : internal(o.internal)
{
}

FeatureManager::~FeatureManager()
{
}

std::string
FeatureManager::getdid(const Document &doc)
{
    return internal->getdid(doc);
}

int
FeatureManager::getlabel(map<string, map<string, int> > qrel2, const Document &doc, std::string & qid)
{
    return internal->getlabel(qrel2, doc, qid);
}

Xapian::RankList
FeatureManager::create_rank_list(const Xapian::MSet & mset, std::string & qid, bool train)
{
    return internal->create_rank_list(mset, qid, train);
}

Xapian::FeatureVector
FeatureManager::create_feature_vector(map<int,double> fvals, int &label, Xapian::docid & did)
{
    return internal->create_feature_vector(fvals, label, did);
}

map<string, map<string,int> >
FeatureManager::load_relevance(const std::string & qrel_file)
{
    return internal->load_relevance(qrel_file);
}

std::map<int,double>
FeatureManager::transform(const Document &doc, double &weight_)
{
    return internal->transform(doc, weight_);
}

void
FeatureManager::set_database(const Database &db)
{
    internal->letor_db = db;
    internal->update_collection_level();
}

void
FeatureManager::set_query(const Query &query)
{
    internal->letor_query = query;
    internal->update_query_level();
}

}
