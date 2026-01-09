
import express, { Router } from 'express';
import { NodeController } from '../../controllers/NodeController';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';


const router = express.Router();
const nodeController = new NodeController();
router.get("/ping", asyncHandlerMiddleware(nodeController.ping.bind(nodeController)));

router.post("/echo", asyncHandlerMiddleware(nodeController.echo.bind(nodeController)));

router.get("/status", asyncHandlerMiddleware(nodeController.status.bind(nodeController)));

router.get("/", asyncHandlerMiddleware(nodeController.getNodes.bind(nodeController)));

router.post("/", asyncHandlerMiddleware(nodeController.createNode.bind(nodeController)));

router.put("/:id", asyncHandlerMiddleware(nodeController.updateNode.bind(nodeController)));


export default router;