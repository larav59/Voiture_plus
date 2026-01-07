
import express from 'express';
import { asyncHandlerMiddleware } from '../../middlewares/asyncHandlerMiddleware';
import { NodeTypeController } from '../../controllers/NodeTypeController';


const router = express.Router();
const nodeTypeController = new NodeTypeController();
router.get("/ping", asyncHandlerMiddleware(nodeTypeController.ping.bind(nodeTypeController)));

router.post("/echo", asyncHandlerMiddleware(nodeTypeController.echo.bind(nodeTypeController)));

router.get("/status", asyncHandlerMiddleware(nodeTypeController.status.bind(nodeTypeController)));

router.get("/", asyncHandlerMiddleware(nodeTypeController.getNodeTypes.bind(nodeTypeController)));

router.post("/", asyncHandlerMiddleware(nodeTypeController.createNodeType.bind(nodeTypeController)));

router.put("/:id", asyncHandlerMiddleware(nodeTypeController.updateNodeType.bind(nodeTypeController)));

export default router;