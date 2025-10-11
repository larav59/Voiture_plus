import express from 'express';
import testRoutes from './testRoute';

const router = express.Router();

// Toutes les routes pour la version 1 de l'API seront définies ici.
router.use("/test", testRoutes);

export default router;