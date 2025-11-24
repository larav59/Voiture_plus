import { Request, Response } from "express";
import { Authenticated } from "../domain/auth/UserAuth";
import { HttpStatusEnum } from "../infrastructure/webserver";
import { NotFound, NotImplemented } from "../infrastructure/webserver/errors/HttpError";
import { 
    LoginRequest
} from "../domain/requests/Login";
import { LoginService } from "../domain/services/LoginService";
import jwt from "jsonwebtoken";

export class LoginController {
    
    async ping(req: Request, res: Response) {
        res.status(HttpStatusEnum.OK).json({ message: "pong" });
    }

    public async login(req: Request, res: Response) : Promise<void> {
        const request = LoginRequest.fromRequest(req);
		const loginService = new LoginService();

		if (request.validate().hasErrors()) {
			throw new NotFound("Invalid Request",request.validate().getErrors());
		}

		const userExist = await loginService.login(request.username, request.password);
        if (!userExist) {
            res.status(HttpStatusEnum.UNAUTHORIZED).json({ message: "Invalid username or password" });
            return;
        }

        const token = jwt.sign(
            { username: request.username },
            process.env.JWT_SECRET || "supervision_secret",
            { expiresIn: '1h' }
        );

        res.status(HttpStatusEnum.OK).json({ token });
		
    }

    @Authenticated()
    async echo(req: Request, res: Response) {
        res.json({
            identity: req.identity,
            body: req.body
        });
    }

    async status(req: Request, res: Response) {
        throw new NotImplemented("Status endpoint not implemented yet");
    }
}
