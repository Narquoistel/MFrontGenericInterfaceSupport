/*!
 * \file   bindings/julia/src/MaterialDataManager.cxx
 * \brief
 * \author Thomas Helfer
 * \date   19/05/2019
 * \copyright (C) Copyright Thomas Helfer 2018.
 * Use, modification and distribution are subject
 * to one of the following licences:
 * - GNU Lesser General Public License (LGPL), Version 3.0. (See accompanying
 *   file LGPL-3.0.txt)
 * - CECILL-C,  Version 1.0 (See accompanying files
 *   CeCILL-C_V1-en.txt and CeCILL-C_V1-fr.txt).
 */

#include <jlcxx/jlcxx.hpp>
#include "MGIS/Behaviour/MaterialDataManager.hxx"
#include "MGIS/Julia/JuliaUtilities.hxx"

void declareMaterialDataManager();

void declareMaterialDataManager(jlcxx::Module& m) {
  m.add_type<mgis::behaviour::MaterialDataManager>("MaterialDataManager");
}  // end of declareMaterialDataManager
