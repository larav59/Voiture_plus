import { Validator } from "../../../infrastructure/validator/Validator";
import { BaseRequest } from "../BaseRequest";

export class LoginRequest extends BaseRequest<LoginRequest> {
    username : string ;
    password : string ;

    constructor(fields?: Partial<LoginRequest>) {
        super();
        this.username = fields?.username ?? "";
        this.password = fields?.password ?? "";
    }

    // Méthode pour la validation
    public validate(): Validator<LoginRequest> {
        const validator = new Validator<LoginRequest>(this);
        validator.field("username").isRequired().maxLength(100);
        validator.field("password").isRequired().maxLength(100);
        return validator;
    }

    static fromRequest(req: any): LoginRequest {
        const body = req.body
        body.username = body.username !== undefined ? String(body.username) : undefined;
        body.password = body.password !== undefined ? String(body.password) : undefined;

        return new LoginRequest({
            username: body.username,
            password: body.password,
        });
    }
}