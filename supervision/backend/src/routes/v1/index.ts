import express from 'express';
import testRoutes from './testRoute';
import vehiculeRoutes from './vehicleRoute'

const router = express.Router();

// Toutes les routes pour la version 1 de l'API seront définies ici.
router.use("/test", testRoutes);
router.use("/vehicules", vehiculeRoutes);

export default router;