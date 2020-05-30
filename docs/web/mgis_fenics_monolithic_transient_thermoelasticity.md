---
title: Monolithic transient thermo-elasticity
author:
  - Jérémy Bleyer
  - Thomas Helfer
date: 2020
lang: en-EN
numbersections: true
toc: true
from: markdown+tex_math_single_backslash
table-of-contents: true
geometry: margin=2cm
link-citations: true
colorlinks: true
figPrefixTemplate: "$$i$$"
tblPrefixTemplate: "$$i$$"
secPrefixTemplate: "$$i$$"
eqnPrefixTemplate: "($$i$$)"
highlight-style: tango
bibliography: bibliography.bib
monofont: DejaVuSansMono.ttf 
mathfont: texgyredejavu-math.otf
---

\newcommand{\bsig}{\boldsymbol{\sigma}}
\newcommand{\beps}{\boldsymbol{\varepsilon}}
\newcommand{\bj}{\mathbf{j}}

This demo is a direct transposition of the [transient thermo-elasticity
demo](https://comet-fenics.readthedocs.io/en/latest/demo/thermoelasticity/thermoelasticity_transient.html)
using a pure `FEniCS` formulation. We will show how to compute fully
coupled thermo-mechanical problems using `MFront`, which can pave the
way to more complex thermo-mechanical behaviours including plasticity
for instance.

> **Source files:**
>
> * Jupyter notebook: [mgis_fenics_monolithic_transient_thermoelasticity.ipynb](https://gitlab.enpc.fr/navier-fenics/mgis-fenics-demos/raw/master/demos/transient_thermoelasticity/mgis_fenics_monolithic_transient_thermoelasticity.ipynb)
> * Python file: [mgis_fenics_monolithic_transient_thermoelasticity.py](https://gitlab.enpc.fr/navier-fenics/mgis-fenics-demos/raw/master/demos/transient_thermoelasticity/mgis_fenics_monolithic_transient_thermoelasticity.py)
> * MFront behaviour file: [ThermoElasticity.mfront](https://gitlab.enpc.fr/navier-fenics/mgis-fenics-demos/raw/master/demos/transient_thermoelasticity/ThermoElasticity.mfront)

# Constitutive equations

The constitutive equations are derived from the following expression of
the Gibbs free energy:

$$
\begin{aligned}
\rho\,\Phi{\left(\beps^{\mathrm{to}},T\right)}&={{\displaystyle \frac{\displaystyle \lambda}{\displaystyle 2}}}\,{\left({\mathrm{tr}{\left(\beps^{\mathrm{to}}\right)}}-3\,\alpha\,{\left(T-T^{\mathrm{ref}}\right)}\right)}^2+
\mu\,{\left(\beps^{\mathrm{to}}-\alpha\,{\left(T-T^{\mathrm{ref}}\right)}\,\mathbf{I}\right)}\,\colon\,{\left(\beps^{\mathrm{to}}-\alpha\,{\left(T-T^{\mathrm{ref}}\right)}\,\mathbf{I}\right)}\\
&+{{\displaystyle \frac{\displaystyle \rho\,C_{\varepsilon}}{\displaystyle 2\,T^{\mathrm{ref}}}}}\,{\left(T-T^{\mathrm{ref}}\right)}^2+s_{0}\,{\left(T-T^{\mathrm{ref}}\right)}
\end{aligned}
$$

where:

- $\lambda$ and $\mu$ are the Lamé coefficients
- $\rho$ is the mass density
- $\alpha$ is mean linear thermal expansion coefficient
- $C_{\varepsilon}$ is the specific heat at constant strain (per unit
  of mass).

This expression leads to the following expressions of the stress tensor
$\bsig$ and entropy per unit of mass $s$:

$$
\begin{aligned}
\bsig&=\rho \dfrac{\partial \Phi}{\partial \beps^{\mathrm{to}}}=\lambda\,{\mathrm{tr}{\left(\beps^{\mathrm{to}}\right)}}\,\mathbf{I}+2\,\mu\,\beps^{\mathrm{to}}-\kappa\,{\left(T-T^{\mathrm{ref}}\right)}\,\mathbf{I}\\
s&={\displaystyle \frac{\displaystyle \partial \Phi}{\displaystyle \partial T}}={{\displaystyle \frac{\displaystyle C_{\varepsilon}}{\displaystyle T^{\mathrm{ref}}}}}\,{\left(T-T^{\mathrm{ref}}\right)}+{{\displaystyle \frac{\displaystyle \kappa}{\displaystyle \rho}}}\,{\mathrm{tr}{\left(\beps^{\mathrm{to}}\right)}}\\
\end{aligned}
\tag{1}
$${#eq:constitutive_equations}

where $\kappa=\alpha\,{\left(3\,\lambda+2\,\mu\right)}$.

The heat flux $\bj$ is related to the temperature gradient
$\nabla\, T$ by the linear Fourier law:

$$
\bj=-k\,\nabla\, T
\tag{2}
$${#eq:constitutive_equations_2}

# `MFront` implementation

## Choice of the domain specific language

The constitutive equations @eq:constitutive_equations
and @eq:constitutive_equations_2 exhibit an explicit expression of the
thermodynamic forces ${\left(\bsig\, \bj, s\right)}$ as a function of
the gradients ${\left(\beps^{\mathrm{to}}, \nabla T, T\right)}$.

The most suitable domain specific language for this kind of behaviour if
the `DefaultGenericBehaviour`.

``` cxx
@DSL DefaultGenericBehaviour;
```

## Name of the behaviour


The `@Behaviour` keyword allows giving the name of the behaviour:

``` cxx
@Behaviour ThermoElasticity;
```

## Metadata


The following lines add some metadata (authors of the implementation,
date, description):

``` cxx
@Author Thomas Helfer, Jérémy Bleyer;
@Date 19/04/2020;
@Description {
  This simple thermoelastic behaviour allows to perform
  fully coupled thermo-mechanical resolutions.

  See https://comet-fenics.readthedocs.io/ for details.
}
```

## Definition of the gradients and conjugated thermodynamic forces

The gradients are the strain $\beps^{\mathrm{to}}$,
the temperature gradient $\nabla\,T$ and the temperature. The
associated thermodynamic forces are respectively the stress
$\bsig$, the heat flux $\bj$ and the entropy $s$.

$\beps^{\mathrm{to}}$, $\nabla\,T$,
$\bsig$ and $\bj$ are declared as follows:

``` cxx
@Gradient StrainStensor εᵗᵒ;
εᵗᵒ.setGlossaryName("Strain");

@ThermodynamicForce StressStensor σ;
σ.setGlossaryName("Stress");

@Gradient TemperatureGradient ∇T;
∇T.setGlossaryName("TemperatureGradient");

@ThermodynamicForce HeatFlux j;
j.setGlossaryName("HeatFlux");
```

The glossary names are the names seen from the calling solver. Glossary
names are described on [this
page](http://tfel.sourceforge.net/glossary.html).

Due to a `MFront` convention, the temperature is automatically declared
as an external state variable. For this reason, the entropy is declared
as a state variable:

``` cxx
@StateVariable real s;
s.setEntryName("EntropyPerUnitOfMass");
```

In the current version of `MFront`, there is no glossary name associated
with the entropy per unit of mass. In this case, the `setEntryName` is
used to associate a name to this variable.

## Declaration of the tangent operator blocks

By default, all the derivatives of the thermodynamic forces with respect
to the increments of the gradients are declared as tangent operator
blocks, i.e. derivatives that are meant to be used when building the
stiffness matrix at the structural scale.

In this case, this is not appropriate as:

-   some derivatives are known to be null, such as
    ${\displaystyle \frac{\displaystyle \partial \bsig}{\displaystyle \partial \Delta\,\nabla\,T}}$
    and
    ${\displaystyle \frac{\displaystyle \partial \bj}{\displaystyle \partial \Delta\,\beps^{\mathrm{to}}}}$.
-   the derivative
    ${\displaystyle \frac{\displaystyle \partial s}{\displaystyle \partial \Delta\,\beps^{\mathrm{to}}}}$
    of the entropy with respect to strain and the derivative
    ${\displaystyle \frac{\displaystyle \partial s}{\displaystyle \partial \Delta\,T}}$
    of the entropy with respect to the temperature are also required.

The required tangent operator blocks are therefore explicitly requested:

``` cxx
@TangentOperatorBlocks{∂σ∕∂Δεᵗᵒ, ∂σ∕∂ΔT, ∂s∕∂ΔT, ∂s∕∂Δεᵗᵒ, ∂j∕∂Δ∇T};
```

## Declaration of the reference temperature

The reference temperature is declared using the `@StaticVariable`
keyword:

``` cxx
@StaticVariable temperature Tʳᵉᶠ = 293.15;
```

Internally `Tʳᵉᶠ` is hold in an immutable static variable.

## Declaration of the material coefficients

The various material coefficients are now declared as parameters:

``` cxx
@Parameter stress E = 70e3;
E.setGlossaryName("YoungModulus");
@Parameter real ν = 0.3;
ν.setGlossaryName("PoissonRatio");
@Parameter massdensity ρ = 2700.;
ρ.setGlossaryName("MassDensity");
@Parameter thermalconductivity α = 2.31e-5 ;
α.setGlossaryName("ThermalExpansion");
@Parameter real Cₑ = 910e-6;
Cₑ.setEntryName("SpecificHeatAtConstantStrainPerUnitOfMass");
@Parameter thermalconductivity k = 237e-6;
k.setGlossaryName("ThermalConductivity");
```

Parameters are global values that can be modified at runtime.

## Computation of the thermodynamic forces and tangent operator blocks


The computation of the thermodynamic forces and tangent operator blocks
is implemented in the `@Integrator` code block:

``` cxx
@Integrator{
```

First, the Lamé coefficients are computed using the built-in
`computeLambda` and `computeMu` functions and then we compute the
$\kappa$ factor:

``` cxx
  const auto λ = computeLambda(E, ν);
  const auto μ = computeMu(E, ν);
  const auto κ = α ⋅ (2 ⋅ μ + 3 ⋅ λ);
```

For brevity, we compute the strain at the end of the time step as
follows:

``` cxx
  const auto ε = εᵗᵒ + Δεᵗᵒ;
```

The computation of the thermodynamic forces is then straightforward and
closely looks like the constitutive equations @eq:constitutive_equations
and @eq:constitutive_equations_2:

``` cxx
  σ = λ ⋅ trace(ε) ⋅ I₂ + 2 ⋅ μ ⋅ ε - κ ⋅ (T + ΔT - Tʳᵉᶠ) ⋅ I₂;
  s = Cₑ / Tʳᵉᶠ ⋅ (T + ΔT - Tʳᵉᶠ) + (κ / ρ) ⋅ trace(ε);
  j = -k ⋅ (∇T + Δ∇T);
```

The computation of the consistent tangent operator is only required if
the `computeTangentOperator_` boolean value is true. Again, their
computations is straightforward [2]:

``` cxx
  if (computeTangentOperator_) {
    ∂σ∕∂Δεᵗᵒ = λ ⋅ (I₂ ⊗ I₂) + 2 ⋅ μ ⋅ I₄;
    ∂σ∕∂ΔT = -κ ⋅ I₂;
    ∂s∕∂ΔT = Cₑ / Tʳᵉᶠ;
    ∂s∕∂Δεᵗᵒ = κ ⋅ Cₑ / Tʳᵉᶠ ⋅ I₂;
    ∂j∕∂Δ∇T = -k ⋅ tmatrix<N, N, real>::Id();
  }
```

A final curly bracket then ends the `@Integrator` code block:

``` cxx
}
```

[1] We may also note that those blocks are third order tensors that are
not yet supported by `MFront`.

[2] `N` is the space dimension. `real` is a type alias to the numeric
type used, which depends on the interface used.

# `FEniCS` implementation

## Problem position

The problem consists of a quarter of a square plate perforated by a
circular hole. A temperature increase of $\Delta T=+10^{\circ}\text{C}$
will be applied on the hole boundary. Symmetry conditions are applied on
the corresponding symmetry planes and stress and flux-free boundary
conditions are adopted on the plate outer boundary. Similarly to the
[original
demo](https://comet-fenics.readthedocs.io/en/latest/demo/thermoelasticity/thermoelasticity_transient.html),
we will formulate the problem using the temperature variation as the
main unknown.

We first import the relevant modules then define the mesh and some constants.


```python
from dolfin import *
import mgis.fenics as mf
from mshr import Rectangle, Circle, generate_mesh
import numpy as np
import matplotlib.pyplot as plt

L = 1.0
R = 0.1
N = 50  # mesh density

domain = Rectangle(Point(0.0, 0.0), Point(L, L)) - \
    Circle(Point(0.0, 0.0), R, 100)
mesh = generate_mesh(domain, N)

Tref = Constant(293.15)
DThole = Constant(10.0)
dt = Constant(0)  # time step
```

We now define the relevant FunctionSpace for the considered problem.
Since we will adopt a monolithic approach i.e. in which both fields are
coupled and solved at the same time, we will need to resort to a Mixed
FunctionSpace for both the displacement $\boldsymbol{u}$ and the
temperature variation $\Theta = T-T^\text{ref}$.


```python
Vue = VectorElement("CG", mesh.ufl_cell(), 2)  # displacement finite element
Vte = FiniteElement("CG", mesh.ufl_cell(), 1)  # temperature finite element
V = FunctionSpace(mesh, MixedElement([Vue, Vte]))


def inner_boundary(x, on_boundary):
    return near(x[0]**2 + x[1]**2, R**2, 1e-3) and on_boundary


def bottom(x, on_boundary):
    return near(x[1], 0) and on_boundary


def left(x, on_boundary):
    return near(x[0], 0) and on_boundary


bcs = [DirichletBC(V.sub(0).sub(1), Constant(0.0), bottom),
       DirichletBC(V.sub(0).sub(0), Constant(0.0), left),
       DirichletBC(V.sub(1), DThole, inner_boundary)]
```

## Variational formulation and time discretization

The constitutive equations described earlier are completed by the
quasi-static equilibrium equation:

$$
\text{div} \bsig = 0
$$

and the transient heat equation (without source terms):

$$
\rho T^\text{ref} \dfrac{\partial s}{\partial t} + \text{div} \bj= 0
$$

which can both be written in the following weak form:

$$
\begin{aligned}
\int_{\Omega}\bsig :\nabla^s\widehat{\boldsymbol{u}}\text{ d} \Omega &=\int_{\partial \Omega} (\bsig\cdot\boldsymbol{n})\cdot\widehat{\boldsymbol{u}} dS \quad \forall \widehat{\boldsymbol{u}}\in V_U \\
\int_{\Omega}\rho T^\text{ref} \dfrac{\partial s}{\partial t}\widehat{T}d\Omega - \int_{\Omega} \bj\cdot\nabla \widehat{T}d\Omega &= -\int_{\partial \Omega} \bj\cdot\boldsymbol{n} \widehat{T} dS \quad \forall \widehat{T} \in V_T
\end{aligned} \tag{3}
$${#eq:coupled-system}

with $V_U$ and $V_T$ being the displacement and temperature function spaces.

The time derivative in the heat equation is now replaced by an implicit
Euler scheme, so that the previous weak form at the time increment $n+1$
is now:

$$
\int_{\Omega}\rho T^\text{ref} \dfrac{s^{n+1}-s^n}{\Delta t}\widehat{T}d\Omega - \int_{\Omega} \bj^{n+1}\cdot\nabla \widehat{T}d\Omega = -\int_{\partial \Omega} \bj^{n+1}\cdot\boldsymbol{n} \widehat{T} dS \quad \forall \widehat{T} \in V_T
$$

where $s^{n+1},\bj^{n+1}$ correspond to the *unknown* entropy and heat
flux at time $t_{n+1}$.

Since both the entropy and the stress tensor depend on the temperature
and the total strain, we obtain a fully coupled problem at $t=t_{n+1}$
for $(\boldsymbol{u}_{n+1},T_{n+1})\in V_U\times V_T$. With the retained
boundary conditions both right-hand sides in
@eq:coupled-system.

We now load the material behaviour and define the corresponding
`MFrontNonlinearProblem`. One notable specificity of the present example
is that the unknown field `v` belongs to a mixed function space.
Therefore, we cannot rely on automatic registration for the strain and
temperature gradient. We will have to specify explicitly their UFL
expression with respect to the displacement `u` and temperature
variation `Theta` sub-functions of the mixed unknown `v`. We also
register the `"Temperature"` external state variable with respect to
`Theta`.

```python
material = mf.MFrontNonlinearMaterial("./src/libBehaviour.so",
                                      "ThermoElasticity",
                                      hypothesis="plane_strain")
rho = Constant(material.get_parameter("MassDensity"))
v = Function(V)
(u, Theta) = split(v)
problem = mf.MFrontNonlinearProblem(v, material, quadrature_degree=2, bcs=bcs)
problem.register_gradient("Strain", sym(grad(u)))
problem.register_gradient("TemperatureGradient", grad(Theta))
problem.register_external_state_variable("Temperature", Theta + Tref)
```

Similarly to the [Transient heat equation with phase change
demo](https://thelfer.github.io/mgis/web/mgis_fenics_heat_equation_phase_change.html),
we need to specify explicitly the coupled thermo-mechanical residual
expression using the stress, heat flux and entropy variables. For the
implicit Euler scheme, we will need to define the entropy at the
previous time step. For the mechanical residual, note that the stress
variable `sig` is represented in the form of its vector of components.
The computation of $\bsig :\nabla^s\widehat{\boldsymbol{u}}$ therefore
requires to express $\widehat{\beps}=\nabla^s\widehat{\boldsymbol{u}}$
in the same way. For this purpose, we could use the
`mgis.fenics.utils.symmetric_tensor_to_vector` on the tensorial UFL
expression `sym(grad(u))`. Another possibility is to get the
corresponding `"Strain"` gradient object (expressed in vectorial form)
and get his variation with respect to `v_`.


```python
sig = problem.get_flux("Stress")
j = problem.get_flux("HeatFlux")
s = problem.get_state_variable("EntropyPerUnitOfMass")
problem.initialize()

s_old = s.copy(deepcopy=True)
v_ = TestFunction(V)
u_, T_ = split(v_)  # Displacement and temperature test functions
eps_ = problem.gradients["Strain"].variation(v_)
mech_residual = dot(sig, eps_)*problem.dx
thermal_residual = (rho*Tref*(s - s_old)/dt*T_ - dot(j, grad(T_)))*problem.dx
problem.residual = mech_residual + thermal_residual
problem.compute_tangent_form()
```

# Resolution

The problem is now solved by looping over time increments. Because of
the typical exponential time variation of temperature evolution of the
heat equation, time steps are discretized on a non-uniform (logarithmic)
scale. $\Delta t$ is therefore updated at each time step. The previous
entropy field `s_old` is updated at the end of each step.


```python
Nincr = 10
t = np.logspace(1, 4, Nincr + 1)
Nx = 100
x = np.linspace(R, L, Nx)
T_res = np.zeros((Nx, Nincr + 1))
for (i, dti) in enumerate(np.diff(t)):
    print("Increment " + str(i + 1))
    dt.assign(dti)
    problem.solve(v.vector())

    s_old.assign(s)

    T_res[:, i + 1] = [v(xi, 0.0)[2] for xi in x]
```

    Increment 1
    Increment 2
    Increment 3
    Increment 4
    Increment 5
    Increment 6
    Increment 7
    Increment 8
    Increment 9
    Increment 10

At each time increment, the variation of the temperature increase
$\Delta T$ along a line $(x, y=0)$ is saved in the `T_res` array. This
evolution is plotted below. As expected, the temperature gradually
increases over time, reaching eventually a uniform value of
$+10^{\circ}\text{C}$ over infinitely long waiting time. We check that
we obtain the same solution as the pure `FEniCS` demo.


```python
%matplotlib notebook

plt.figure()
plt.plot(x, T_res[:, 1::Nincr // 10])
plt.xlabel("$x$-coordinate along $y=0$")
plt.ylabel("Temperature variation $\Delta T$")
plt.legend(["$t={:.0f}$".format(ti) for ti in t[1::Nincr // 10]], ncol=2)
plt.show()
```

    <IPython.core.display.Javascript object>



<img src="data:image/png;base64,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" width="639.85">
