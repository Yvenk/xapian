/* ranker.cc: The abstract ranker file.
 *
 * Copyright (C) 2012 Parth Gupta
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
#include <xapian/intrusive_ptr.h>
#include <xapian/types.h>
#include <xapian/visibility.h>

#include <xapian-letor/ranker.h>
#include <xapian-letor/ranklist.h>
#include <xapian-letor/scorer.h>

#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using namespace Xapian;

struct scoreComparer {
    bool operator()(const pair<Xapian::docid, int>& first_pair, const pair<Xapian::docid, int>& second_pair) const {
        return first_pair.second > second_pair.second;
    }
};

Ranker::Ranker() {
    MAXPATHLEN = 200;
}

Ranker::Ranker(int metric_type) { //TODO: update this when adding scorers
    MAXPATHLEN = 200;
    // switch(metric_type) {
    //     case 0: this -> scorer = new NDCGScorer;
    //             break;
    //     case 1: this -> scorer = new ERRScorer;
    //             break;
    //     default: ;
    (void)metric_type;

}

double
Ranker::get_score(Xapian::RankList & rl){
    return this->scorer->score(rl);
}

std::vector<Xapian::RankList>
Ranker::get_traindata(){
    return this->traindata;
}

void
Ranker::set_training_data(vector<Xapian::RankList> training_data1) {
    this->traindata = training_data1;
}

std::string
Ranker::get_cwd() {
    char temp[MAXPATHLEN];
    return (getcwd(temp, MAXPATHLEN) ? std::string(temp) : std::string(""));
}

void
Ranker::train_model() {
}

void
Ranker::save_model_to_file() {
}

void
Ranker::load_model_from_file(const std::string & model_file) {
    (void)model_file;
}

// TODO: Add definition of rank method

// TODO: add aggregation methods when including scorers
