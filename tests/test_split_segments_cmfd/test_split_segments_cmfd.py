#!/usr/bin/env python

import os
import sys
sys.path.insert(0, os.pardir)
sys.path.insert(0, os.path.join(os.pardir, 'openmoc'))
from testing_harness import TestHarness
from input_set import PinCellInput
import openmoc
import openmoc.plotter as plotter

class SplitSegmentsCMFDTestHarness(TestHarness):
    """Test segment splitting based on max optical path length with
    CMFD turned on."""

    def __init__(self):
        super(SplitSegmentsCMFDTestHarness, self).__init__()
        self.input_set = PinCellInput()

    def _setup(self):
        """Build materials, geometry, CMFD, and perform ray tracing."""
        self._create_geometry()
        self._create_trackgenerator()

        # Overlay simple CMFD mesh
        cmfd = openmoc.Cmfd()
        cmfd.setLatticeStructure(2, 2)
        self.input_set.geometry.setCmfd(cmfd)

        self._generate_tracks()
        self._create_solver()

    def _run_openmoc(self):
        """Set a small max optical path length to ensure segments are split."""

        # Set a small max optical path length so segments are split
        self.solver.setMaxOpticalLength(0.5)

        geometry = self.input_set.geometry
        track_generator = self.track_generator
        solver=self.solver

        plotter.plot_quadrature(solver)
        plotter.plot_tracks(track_generator)
        plotter.plot_segments(track_generator)
        plotter.plot_materials(geometry, gridsize=500)
        plotter.plot_cells(geometry, gridsize=500)
        plotter.plot_flat_source_regions(geometry, gridsize=500)
        import time
        time.sleep(1000)

        super(SplitSegmentsCMFDTestHarness, self)._run_openmoc()

    def _get_results(self):
        """Digest info in the results and return as a string."""
        return super(SplitSegmentsCMFDTestHarness, self).\
            _get_results(num_segments=True, fluxes=False, keff=False)


if __name__ == '__main__':
    harness = SplitSegmentsCMFDTestHarness()
    harness.main()
