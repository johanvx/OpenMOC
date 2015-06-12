/**
 * @file Geometry.h
 * @brief The Geometry class.
 * @date January 19, 2012
 * @author William Boyd, MIT, Course 22 (wboyd@mit.edu)
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#ifdef __cplusplus
#include "Python.h"
#include "Cmfd.h"
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <sstream>
#include <string>
#include <omp.h>
#include <functional>
#ifndef CUDA
  #include <unordered_map>
#endif
#endif

/** Forward declaration of Cmfd class */
class Cmfd;

/**
 * @struct fsr_data
 * @brief A fsr_data struct represents an FSR with a unique FSR ID
 *        and a characteristic point that lies within the FSR that
 *        can be used to recompute the hierarchical LocalCoords
 *        linked list.
 */
struct fsr_data {

  /** The FSR ID */
  int _fsr_id;

  /** Characteristic point in Root Universe that lies in FSR */
  Point* _point;

  /** Global numerical centroid in Root Universe */
  Point* _centroid;
};

void reset_auto_ids();


/**
 * @class Geometry Geometry.h "src/Geometry.h"
 * @brief The master class containing references to all geometry-related
 *        objects - Surfaces, Cells, Universes and Lattices - and Materials.
 * @details The primary purpose for the geometry is to serve as a collection
 *          of all geometry-related objects, as well as for ray tracing
 *          of characteristic tracks across the Geometry and computing
 *          FSR-to-cell offset maps.
 */
class Geometry {

private:

  bool _solve_3D;
  
  omp_lock_t* _num_FSRs_lock;

  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _x_min_bc;

  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _x_max_bc;  

  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _y_min_bc;

  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _y_max_bc;  
  
  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _z_min_bc;

  /** The boundary conditions at the top of the bounding box containing
   *  the Geometry. False is for vacuum and true is for reflective BCs. */
  boundaryType _z_max_bc;
  
  /** The total number of FSRs in the Geometry */
  int _num_FSRs;

  /** An map of FSR key hashes to unique fsr_data structs */
#ifndef CUDA
  std::unordered_map<std::size_t, fsr_data> _FSR_keys_map;
#endif

  /** An vector of FSR key hashes indexed by FSR ID */
  std::vector<std::size_t> _FSRs_to_keys;

  /** A vector of Material IDs indexed by FSR IDs */
  std::vector<int> _FSRs_to_material_IDs;

  /** The maximum Track segment length in the Geometry */
  FP_PRECISION _max_seg_length;

  /** The minimum Track segment length in the Geometry */
  FP_PRECISION _min_seg_length;

  /* The Universe at the root node in the CSG tree */
  Universe* _root_universe;

  /** A CMFD object pointer */
  Cmfd* _cmfd;

  /* A map of all Material in the Geometry for optimization purposes */
  std::map<int, Material*> _all_materials;

  Cell* findFirstCell(LocalCoords* coords, double azim, double polar=M_PI_2);
  Cell* findNextCell(LocalCoords* coords, double azim, double polar=M_PI_2);

public:

  Geometry();
  virtual ~Geometry();

  /* Get parameters */
  double getWidth();
  double getHeight();
  double getDepth();
  double getMinX();
  double getMaxX();
  double getMinY();
  double getMaxY();
  double getMinZ();
  double getMaxZ();
  boundaryType getMinXBoundaryType();
  boundaryType getMaxXBoundaryType();
  boundaryType getMinYBoundaryType();
  boundaryType getMaxYBoundaryType();
  boundaryType getMinZBoundaryType();
  boundaryType getMaxZBoundaryType();
  Universe* getRootUniverse();
  int getNumFSRs();
  int getNumEnergyGroups();
  int getNumMaterials();
  int getNumCells();
  std::map<int, Material*> getAllMaterials();
  std::map<int, Cell*> getAllMaterialCells();
  void setRootUniverse(Universe* root_universe);

  Cmfd* getCmfd();
  std::vector<std::size_t> getFSRsToKeys();
  std::vector<int> getFSRsToMaterialIDs();
  int getFSRId(LocalCoords* coords);
  Point* getFSRPoint(int fsr_id);
  Point* getFSRCentroid(int fsr_id);
  std::string getFSRKey(LocalCoords* coords);
#ifndef CUDA
  std::unordered_map<std::size_t, fsr_data> getFSRKeysMap();
#endif

  /* Set parameters */
  void setFSRsToMaterialIDs(std::vector<int> FSRs_to_material_IDs);
  void setFSRsToKeys(std::vector<std::size_t> FSRs_to_keys);
  void setNumFSRs(int num_fsrs);
  void setCmfd(Cmfd* cmfd);

#ifndef CUDA
  void setFSRKeysMap(std::unordered_map<std::size_t, fsr_data> FSR_keys_map);
#endif

  /* Find methods */
  Cell* findCellContainingCoords(LocalCoords* coords);
  Material* findFSRMaterial(int fsr_id);
  int findFSRId(LocalCoords* coords);
  Cell* findCellContainingFSR(int fsr_id);
  
  /* Other worker methods */
  void subdivideCells();
  void initializeFlatSourceRegions();
  void segmentize2D(Track2D* track, double z_level);
  void segmentize3D(Track3D* track);
  void computeFissionability(Universe* univ=NULL);
  void setFSRCentroid(int fsr, Point* centroid);
  
  std::string toString();
  void printString();
  void initializeCmfd();
  bool withinBounds(LocalCoords* coords);
};

#endif /* GEOMETRY_H_ */
