package main

import (
	"fmt"

	"github.com/labstack/echo/v4"
	"github.com/labstack/echo/v4/middleware"
)

func main() {
	// Echo instance
	e := echo.New()

	// Middleware
	e.Use(middleware.Logger())
	e.Use(middleware.Recover())
	e.Use(middleware.Decompress())
	e.Use(middleware.Gzip())

	// Routes
	e.POST("/", hello)

	// Start server
	e.Logger.Fatal(e.Start(":1323"))

}

func hello(c echo.Context) error {
	m := echo.Map{}
	if err := c.Bind(&m); err != nil {
		return err
	}
	fmt.Println(m)
	return c.JSON(200, m)
}
