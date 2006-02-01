/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* SolutionAction.cc
 *
 * Easy-to use interface to the ZYPP dependency resolver
 *
 * Copyright (C) 2000-2002 Ximian, Inc.
 * Copyright (C) 2005 SUSE Linux Products GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include "zypp/solver/detail/Resolver.h"
#include "zypp/solver/detail/SolutionAction.h"
#include "zypp/CapSet.h"
#include "zypp/base/Logger.h"
#include "zypp/Dependencies.h"


/////////////////////////////////////////////////////////////////////////
namespace zypp
{ ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
  namespace solver
  { /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
    namespace detail
    { ///////////////////////////////////////////////////////////////////

using namespace std;

IMPL_PTR_TYPE(SolutionAction);
IMPL_PTR_TYPE(TransactionSolutionAction);
IMPL_PTR_TYPE(InjectSolutionAction);

//---------------------------------------------------------------------------

SolutionAction::SolutionAction()
{
}


SolutionAction::~SolutionAction()
{
}


//---------------------------------------------------------------------------

ostream&
operator<<( ostream& os, const TransactionSolutionAction & action)
{
    os << "TransactionSolutionAction: ";
    switch (action._action) {
	case KEEP:	os << "Keep"; break;
	case INSTALL:	os << "Install"; break;
	case UPDATE:	os << "Update"; break;
	case REMOVE:	os << "Remove"; break;
	case UNLOCK:	os << "Unlock"; break;	    
    }
    os << " ";
    os << action._item;
    os << endl;
    return os;
}


ostream&
operator<<( ostream& os, const SolutionActionList & actionlist)
{
    for (SolutionActionList::const_iterator iter = actionlist.begin(); iter != actionlist.end(); ++iter) {
	os << (*iter);
	os << endl;
    }
    return os;
}


ostream&
operator<<( ostream& os, const CSolutionActionList & actionlist)
{
    for (CSolutionActionList::const_iterator iter = actionlist.begin(); iter != actionlist.end(); ++iter) {
	os << (*iter);
	os << endl;
    }
    return os;
}

//---------------------------------------------------------------------------

ostream&
operator<<( ostream& os, const InjectSolutionAction & action)
{
    os << "InjectSolutionAction: ";
    os << action._capability;
    os << ", ";
    os << action._kind;
    os << endl;
    return os;
}

//---------------------------------------------------------------------------


bool 
TransactionSolutionAction::execute(Resolver & resolver) const
{
    bool ret = true;
    switch (action()) {
	case KEEP:
	    ret = _item.status().setNoTransact (ResStatus::USER);
	case INSTALL:
	case UPDATE:
	    _item.status().setToBeInstalled (ResStatus::USER);
	    break;
	case REMOVE:
	    _item.status().setToBeUninstalled (ResStatus::USER);
	    break;
	case UNLOCK:
	    ERR << "Not implemented yet" << endl;
	    ret = false;
#warning Unlocking items not implemented
	    break;
	default:
	    ERR << "Wrong TransactionKind" << endl;
	    ret = false;
    }
    return ret;
}

bool
InjectSolutionAction::execute(Resolver & resolver) const
{
    if (_kind == Dep::CONFLICTS) {
	// removing conflict in both resolvables
	Dependencies dependencies = _item.resolvable()->deps();
	CapSet depList = dependencies[Dep::CONFLICTS];
	if (depList.find(_capability) != depList.end())
	{
	    resolver.addIgnoreConflict (_item, _capability);
	}
	dependencies = _otherItem.resolvable()->deps();
	depList = dependencies[Dep::CONFLICTS];
	if (depList.find(_capability) != depList.end())
	{
	    resolver.addIgnoreConflict (_otherItem, _capability);
	}
    } else if (_kind  == Dep::PROVIDES) {
	// removing the requires dependency from the item
	resolver.addIgnoreRequires (_item, _capability);
    } else {
	ERR << "No valid InjectSolutionAction kind found" << endl;
	return false;
    }
#warning Disabling capabilities currently not possible;
//    resolvable->deprecatedSetDeps(dependencies);
    return true;
}

      ///////////////////////////////////////////////////////////////////
    };// namespace detail
    /////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////
  };// namespace solver
  ///////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////////////
};// namespace zypp
/////////////////////////////////////////////////////////////////////////
