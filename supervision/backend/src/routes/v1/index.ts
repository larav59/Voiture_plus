import express from 'express';
import testRoutes from './testRoute';
import vehicleRoutes from './vehicleRoute'
import alarmRoutes from  './alarmRoute';
const router = express.Router();

// Toutes les routes pour la version 1 de l'API seront définies ici.
router.use("/test", testRoutes);
router.use("/vehicles", vehicleRoutes);
router.use("/alarms", alarmRoutes);
export default router;