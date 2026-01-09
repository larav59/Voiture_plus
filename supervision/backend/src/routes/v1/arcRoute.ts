
import express, { Router } from 'express';
import { ArcController } from '../../controllers/ArcController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const arcController = new ArcController();
router.get("/ping", asyncHandlerMiddleware(arcController.ping.bind(arcController)));

router.post("/echo", asyncHandlerMiddleware(arcController.echo.bind(arcController)));

router.get("/status", asyncHandlerMiddleware(arcController.status.bind(arcController)));

router.get("/", asyncHandlerMiddleware(arcController.getArcs.bind(arcController)));

router.post("/", asyncHandlerMiddleware(arcController.createArc.bind(arcController)));

router.put("/:id", asyncHandlerMiddleware(arcController.updateArc.bind(arcController)));
export default router;