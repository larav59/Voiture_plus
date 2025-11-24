import express from 'express';
import testRoutes from './testRoute';
import vehiculeRoutes from './vehicleRoute'
import loginRoutes from './loginRoute';
import alarmRoutes from './alarmRoute';
import originRoutes from './originRoute';
import travelRoutes from './travelRoute';

const router = express.Router();

// Toutes les routes pour la version 1 de l'API seront définies ici.
router.use("/test", testRoutes);
router.use("/vehicles", vehiculeRoutes);
router.use("/login", loginRoutes);
router.use("/alarms", alarmRoutes);
router.use("/origins", originRoutes);
router.use("/travels", travelRoutes);

export default router;