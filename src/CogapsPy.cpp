#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <numpy/numpyconfig.h>
#include <numpy/ndarrayobject.h>
#include "Rpackage/src/GapsRunner.h"
#include "Rpackage/src/math/Random.h"
#include <unistd.h>
#include <iostream>

namespace p = boost::python;
namespace np = boost::python::numpy;

np::ndarray convertToPyMat(const RowMatrix &rmat)
{
    p::tuple dims = p::make_tuple(rmat.nRow(), rmat.nCol());
    np::ndarray pymat = np::empty(dims, np::dtype::get_builtin<float>());
    return pymat;
    for (unsigned i = 0; i < rmat.nRow(); ++i)
    {
        for (unsigned j = 0; j < rmat.nCol(); ++j)
        {
            pymat[i][j] = rmat(i,j);
        }
    }

    return pymat;
}

np::ndarray convertToPyMat(const ColMatrix &cmat)
{
    p::tuple dims = p::make_tuple(cmat.nRow(), cmat.nCol());
    np::ndarray pymat = np::empty(dims, np::dtype::get_builtin<float>());
    return pymat;
    for (unsigned i = 0; i < cmat.nRow(); ++i)
    {
        for (unsigned j = 0; j < cmat.nCol(); ++j)
        {
            pymat[i][j] = cmat(i,j);
        }
    }

    return pymat;
}

p::tuple CoGAPS(std::string dataFilePath)
{
  GapsRunner runner(dataFilePath, false, 3, false,
        std::vector<unsigned>(1));

  gaps::random::setSeed(123);
  runner.recordSeed(123);
  runner.setMaxIterations(1000);
  runner.setSparsity(0.01f, 0.01f, false);
  runner.setMaxGibbsMass(100.f, 100.f);
  runner.setMaxThreads(1);
  runner.setPrintMessages(true);
  runner.setOutputFrequency(250);
  runner.setCheckpointOutFile("gaps_checkpoint.out");
  runner.setCheckpointInterval(0);
  
  GapsResult result = runner.run();

  std::cout << result.Pmean.nCol();
  std::cout << result.Pmean(0,0);
  //return p::make_tuple(0,0);

  for (unsigned i = 0; i < result.Pmean.nRow(); ++i)
  {
    for (unsigned j = 0; j < result.Pmean.nCol(); ++i)
    {
      std::cout << result.Pmean(i,j) << " ";
    }
    std::cout << "\n";
  }
  return p::make_tuple(0,0);
  
  std::cout << "1";
  convertToPyMat(result.Pmean);
  std::cout << "2";
  np::ndarray Amean = convertToPyMat(result.Amean);
  std::cout << "3";
  np::ndarray Psd = convertToPyMat(result.Psd);
  std::cout << "4";
  np::ndarray Asd = convertToPyMat(result.Asd);
  std::cout << "5";

  return p::make_tuple(result.Pmean, result.Amean, result.Psd, result.Asd);
  //return p::make_tuple(convertRowMatrix(result.Pmean), convertColMatrix(result.Amean), convertRowMatrix(result.Psd), convertColMatrix(result.Asd));
}

BOOST_PYTHON_MODULE(CogapsPy)
{
    Py_Initialize();
    p::def("CoGAPS", CoGAPS, "");
}

int main()
{
  CoGAPS("data/GIST.tsv");
  return 0;
}
