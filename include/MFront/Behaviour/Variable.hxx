/*!
 * \file   MFront/Behaviour/Variable.hxx
 * \brief
 * \author Thomas Helfer
 * \date   19/06/2018
 * \copyright (C) Copyright Thomas Helfer 2018.
 * Use, modification and distribution are subject
 * to one of the following licences:
 * - GNU Lesser General Public License (LGPL), Version 3.0. (See accompanying
 *   file LGPL-3.0.txt)
 * - CECILL-C,  Version 1.0 (See accompanying files
 *   CeCILL-C_V1-en.txt and CeCILL-C_V1-fr.txt).
 */

#ifndef LIB_MFRONT_BEHAVIOUR_VARIABLE_HXX
#define LIB_MFRONT_BEHAVIOUR_VARIABLE_HXX

#include <string>
#include <vector>
#include "MFront/Config.hxx"
#include "MFront/Behaviour/Hypothesis.hxx"

namespace mfront {

  namespace behaviour {

    /*!
     * \brief structure describing a variable
     */
    struct Variable {
      //! name of the variable
      std::string name;
      //! type of the variable
      enum { SCALAR, VECTOR, STENSOR, TENSOR } type;
    };  // end of struct Description

    /*!
     * \return the size of a variable
     * \param[in] v: variable
     * \param[in] h: modelling hypothesis
     */
    MFRONT_EXPORT size_type getVariableSize(const std::vector<Variable> &,
                                            const Hypothesis);
    /*!
     * \return the size of an array that may contain the values described by the
     * given array of variables
     * \param[in] vs: variables
     * \param[in] h: modelling hypothesis
     */
    MFRONT_EXPORT size_type getArraySize(const std::vector<Variable>&,
                                         const Hypothesis);
    /*!
     * \return the offset of the given variable for the given hypothesis
     * \param[in] vs: variables
     * \param[in] n: variable name
     * \param[in] h: modelling hypothesis
     */
    MFRONT_EXPORT size_type getVariableOffset(const std::vector<Variable>&,
                                              const std::string&,
                                              const Hypothesis);

  }  // end of namespace behaviour

}  // end of namespace mfront

#endif /* LIB_MFRONT_BEHAVIOUR_VARIABLE_HXX */
