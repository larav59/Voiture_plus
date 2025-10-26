import { Column, Entity, OneToMany, PrimaryGeneratedColumn } from "typeorm";
import { Travels } from "./Travels";

@Entity("USERS", { schema: "supervision" })
export class Users {
  @PrimaryGeneratedColumn({ type: "int", name: "id" })
  public id!: number;

  @Column("varchar", { name: "username", nullable: true, length: 255 })
  public username!: string | null;

  @Column("varchar", { name: "password", nullable: true, length: 255 })
  public password!: string | null;

  @OneToMany(() => Travels, (travels) => travels.orderedBy2)
  public travels!: Travels[];

  public constructor(init?: Partial<Users>) {
    Object.assign(this, init);
  }
}
